#include "GeneratePackage/GenerateProjectTool.h"
#include "Editor/UnrealEdEngine.h"
#include "EditorAnalytics.h"
#include "FileHelpers.h"
#include "GameMapsSettings.h"
#include "GameProjectGenerationModule.h"
#include "HotUpdateEditorFunLib.h"
#include "InstalledPlatformInfo.h"
#include "IUATHelperModule.h"
#include "PlatformInfo.h"
#include "SourceCodeNavigation.h"
#include "UnrealEdGlobals.h"
#include "AnalyticsEventAttribute.h"
#include "DerivedDataCacheInterface.h"
#include "DesktopPlatformModule.h"
#include "Interfaces/IMainFrameModule.h"
#include "Interfaces/IProjectTargetPlatformEditorModule.h"
#include "Misc/DataDrivenPlatformInfoRegistry.h"
#include "Settings/EditorExperimentalSettings.h"
#include "Settings/ProjectPackagingSettings.h"

#define LOCTEXT_NAMESPACE "FHotUpdateEditorToolModule"

FString GetCookingOptionalParams()
{
	FString OptionalParams;
	const UProjectPackagingSettings* const PackagingSettings = GetDefault<UProjectPackagingSettings>();

	if (PackagingSettings->bSkipEditorContent)
	{
		OptionalParams += TEXT(" -SkipCookingEditorContent");
	}

	if (FDerivedDataCacheInterface* DDC = GetDerivedDataCache())
	{
		OptionalParams += FString::Printf(TEXT(" -ddc=%s"), DDC->GetGraphName());
	}

	return OptionalParams;
}

const TCHAR* GetUATCompilationFlags()
{
	// We never want to compile editor targets when invoking UAT in this context.
	// If we are installed or don't have a compiler, we must assume we have a precompiled UAT.
	return TEXT("-nocompileeditor");
}

void FGenerateProjectTool::PackageProject( const FString& OutPath, const FName InPlatformInfoName, TFunction<void(FString, double)> Callback)
{
	if (!GUnrealEd)
	{
		return;
	}

	GUnrealEd->CancelPlayingViaLauncher();
	{
		const bool bPromptUserToSave = false;
		const bool bSaveMapPackages = true;
		const bool bSaveContentPackages = true;
		const bool bFastSave = false;
		const bool bNotifyNoPackagesSaved = false;
		const bool bCanBeDeclined = false;
		FEditorFileUtils::SaveDirtyPackages(bPromptUserToSave, bSaveMapPackages, bSaveContentPackages, bFastSave,
		                                    bNotifyNoPackagesSaved, bCanBeDeclined);
	}

	// does the project have any code?
	FGameProjectGenerationModule& GameProjectModule = FModuleManager::LoadModuleChecked<FGameProjectGenerationModule>(
		TEXT("GameProjectGeneration"));
	bool bProjectHasCode = GameProjectModule.Get().ProjectHasCodeFiles();

	const PlatformInfo::FPlatformInfo* const PlatformInfo = PlatformInfo::FindPlatformInfo(InPlatformInfoName);
	check(PlatformInfo);

	if (FInstalledPlatformInfo::Get().IsPlatformMissingRequiredFile(PlatformInfo->BinaryFolderName))
	{
		if (!FInstalledPlatformInfo::OpenInstallerOptions())
		{
			FMessageDialog::Open(EAppMsgType::Ok,
			                     LOCTEXT("MissingPlatformFilesPackage",
			                             "Missing required files to package this platform."));
		}
		return;
	}

	if (UGameMapsSettings::GetGameDefaultMap().IsEmpty())
	{
		FMessageDialog::Open(EAppMsgType::Ok,
		                     LOCTEXT("MissingGameDefaultMap",
		                             "No Game Default Map specified in Project Settings > Maps & Modes."));
		return;
	}

	if (PlatformInfo->SDKStatus == PlatformInfo::EPlatformSDKStatus::NotInstalled || (bProjectHasCode && PlatformInfo->
		bUsesHostCompiler && !FSourceCodeNavigation::IsCompilerAvailable()))
	{
		IMainFrameModule& MainFrameModule = FModuleManager::GetModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
		MainFrameModule.BroadcastMainFrameSDKNotInstalled(PlatformInfo->TargetPlatformName.ToString(),
		                                                  PlatformInfo->SDKTutorial);
		TArray<FAnalyticsEventAttribute> ParamArray;
		ParamArray.Add(FAnalyticsEventAttribute(TEXT("Time"), 0.0));
		FEditorAnalytics::ReportEvent(TEXT("Editor.Package.Failed"), PlatformInfo->TargetPlatformName.ToString(),
		                              bProjectHasCode, EAnalyticsErrorCodes::SDKNotFound, ParamArray);
		return;
	}

	UProjectPackagingSettings* PackagingSettings = Cast<UProjectPackagingSettings>(
		UProjectPackagingSettings::StaticClass()->GetDefaultObject());
	const UProjectPackagingSettings::FConfigurationInfo& ConfigurationInfo =
		UProjectPackagingSettings::ConfigurationInfo[PackagingSettings->BuildConfiguration];
	bool bAssetNativizationEnabled = (PackagingSettings->BlueprintNativizationMethod !=
		EProjectPackagingBlueprintNativizationMethod::Disabled);

	const ITargetPlatform* const Platform = GetTargetPlatformManager()->FindTargetPlatform(
		PlatformInfo->TargetPlatformName.ToString());
	{
		if (Platform)
		{
			FString NotInstalledTutorialLink;
			FString DocumentationLink;
			FText CustomizedLogMessage;

			int32 Result = Platform->CheckRequirements(bProjectHasCode, ConfigurationInfo.Configuration,
			                                           bAssetNativizationEnabled, NotInstalledTutorialLink,
			                                           DocumentationLink, CustomizedLogMessage);

			// report to analytics
			FEditorAnalytics::ReportBuildRequirementsFailure(
				TEXT("Editor.Package.Failed"), PlatformInfo->TargetPlatformName.ToString(), bProjectHasCode, Result);

			// report to main frame
			bool UnrecoverableError = false;

			// report to message log
			if ((Result & ETargetPlatformReadyStatus::SDKNotFound) != 0)
			{
				AddMessageLog(
					LOCTEXT("SdkNotFoundMessage", "Software Development Kit (SDK) not found."),
					CustomizedLogMessage.IsEmpty()
						? FText::Format(LOCTEXT("SdkNotFoundMessageDetail",
						                        "Please install the SDK for the {0} target platform!"), Platform->
						                DisplayName())
						: CustomizedLogMessage,
					NotInstalledTutorialLink,
					DocumentationLink
				);
				UnrecoverableError = true;
			}

			if ((Result & ETargetPlatformReadyStatus::LicenseNotAccepted) != 0)
			{
				AddMessageLog(
					LOCTEXT("LicenseNotAcceptedMessage", "License not accepted."),
					CustomizedLogMessage.IsEmpty()
						? LOCTEXT("LicenseNotAcceptedMessageDetail",
						          "License must be accepted in project settings to deploy your app to the device.")
						: CustomizedLogMessage,
					NotInstalledTutorialLink,
					DocumentationLink
				);

				UnrecoverableError = true;
			}

			if ((Result & ETargetPlatformReadyStatus::ProvisionNotFound) != 0)
			{
				AddMessageLog(
					LOCTEXT("ProvisionNotFoundMessage", "Provision not found."),
					CustomizedLogMessage.IsEmpty()
						? LOCTEXT("ProvisionNotFoundMessageDetail",
						          "A provision is required for deploying your app to the device.")
						: CustomizedLogMessage,
					NotInstalledTutorialLink,
					DocumentationLink
				);
				UnrecoverableError = true;
			}

			if ((Result & ETargetPlatformReadyStatus::SigningKeyNotFound) != 0)
			{
				AddMessageLog(
					LOCTEXT("SigningKeyNotFoundMessage", "Signing key not found."),
					CustomizedLogMessage.IsEmpty()
						? LOCTEXT("SigningKeyNotFoundMessageDetail",
						          "The app could not be digitally signed, because the signing key is not configured.")
						: CustomizedLogMessage,
					NotInstalledTutorialLink,
					DocumentationLink
				);
				UnrecoverableError = true;
			}

			if ((Result & ETargetPlatformReadyStatus::ManifestNotFound) != 0)
			{
				AddMessageLog(
					LOCTEXT("ManifestNotFound", "Manifest not found."),
					CustomizedLogMessage.IsEmpty()
						? LOCTEXT("ManifestNotFoundMessageDetail",
						          "The generated application manifest could not be found.")
						: CustomizedLogMessage,
					NotInstalledTutorialLink,
					DocumentationLink
				);
				UnrecoverableError = true;
			}

			if ((Result & ETargetPlatformReadyStatus::RemoveServerNameEmpty) != 0
				&& (bProjectHasCode || (Result & ETargetPlatformReadyStatus::CodeBuildRequired)
					|| (!FApp::GetEngineIsPromotedBuild() && !FApp::IsEngineInstalled())))
			{
				AddMessageLog(
					LOCTEXT("RemoveServerNameNotFound", "Remote compiling requires a server name. "),
					CustomizedLogMessage.IsEmpty()
						? LOCTEXT("RemoveServerNameNotFoundDetail",
						          "Please specify one in the Remote Server Name settings field.")
						: CustomizedLogMessage,
					NotInstalledTutorialLink,
					DocumentationLink
				);
				UnrecoverableError = true;
			}

			if ((Result & ETargetPlatformReadyStatus::CodeUnsupported) != 0)
			{
				FMessageDialog::Open(EAppMsgType::Ok,
				                     LOCTEXT("NotSupported_SelectedPlatform",
				                             "Sorry, packaging a code-based project for the selected platform is currently not supported. This feature may be available in a future release."));
				UnrecoverableError = true;
			}
			else if ((Result & ETargetPlatformReadyStatus::PluginsUnsupported) != 0)
			{
				FMessageDialog::Open(EAppMsgType::Ok,
				                     LOCTEXT("NotSupported_ThirdPartyPlugins",
				                             "Sorry, packaging a project with third-party plugins is currently not supported for the selected platform. This feature may be available in a future release."));
				UnrecoverableError = true;
			}

			if (UnrecoverableError)
			{
				return;
			}
		}
	}

	if (!FModuleManager::LoadModuleChecked<IProjectTargetPlatformEditorModule>("ProjectTargetPlatformEditor").
		ShowUnsupportedTargetWarning(PlatformInfo->VanillaPlatformName))
	{
		return;
	}

	// let the user pick a target directory
	if (OutPath.IsEmpty())
	{
		PackagingSettings->StagingDirectory.Path = FPaths::ProjectDir();
	}
	else
	{
		PackagingSettings->StagingDirectory.Path = OutPath;
	}

	PackagingSettings->SaveConfig();

	// create the packager process
	FString OptionalParams;

	if (PackagingSettings->FullRebuild)
	{
		OptionalParams += TEXT(" -clean");
	}

	if (PackagingSettings->bCompressed)
	{
		OptionalParams += TEXT(" -compressed");
	}

	OptionalParams += GetCookingOptionalParams();

	if (PackagingSettings->bUseIoStore)
	{
		OptionalParams += TEXT(" -iostore");

		// Pak file(s) must be used when using container file(s)
		PackagingSettings->UsePakFile = true;
	}

	if (PackagingSettings->UsePakFile)
	{
		OptionalParams += TEXT(" -pak");
	}

	if (PackagingSettings->bUseIoStore)
	{
		OptionalParams += TEXT(" -iostore");
	}

	if (PackagingSettings->bMakeBinaryConfig)
	{
		OptionalParams += TEXT(" -makebinaryconfig");
	}

	if (PackagingSettings->IncludePrerequisites)
	{
		OptionalParams += TEXT(" -prereqs");
	}

	if (!PackagingSettings->ApplocalPrerequisitesDirectory.Path.IsEmpty())
	{
		OptionalParams += FString::Printf(
			TEXT(" -applocaldirectory=\"%s\""), *(PackagingSettings->ApplocalPrerequisitesDirectory.Path));
	}
	else if (PackagingSettings->IncludeAppLocalPrerequisites)
	{
		OptionalParams += TEXT(" -applocaldirectory=\"$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies\"");
	}

	if (PackagingSettings->ForDistribution)
	{
		OptionalParams += TEXT(" -distribution");
	}

	if (!PackagingSettings->IncludeDebugFiles)
	{
		OptionalParams += TEXT(" -nodebuginfo");
	}

	if (PackagingSettings->bGenerateChunks)
	{
		OptionalParams += TEXT(" -manifests");
	}

	bool bTargetPlatformCanUseCrashReporter = PlatformInfo->bTargetPlatformCanUseCrashReporter;
	if (bTargetPlatformCanUseCrashReporter && PlatformInfo->TargetPlatformName == FName("WindowsNoEditor") &&
		PlatformInfo->PlatformFlavor == TEXT("Win32"))
	{
		FString MinumumSupportedWindowsOS;
		GConfig->GetString(
			TEXT("/Script/WindowsTargetPlatform.WindowsTargetSettings"), TEXT("MinimumOSVersion"),
			MinumumSupportedWindowsOS, GEngineIni);
		if (MinumumSupportedWindowsOS == TEXT("MSOS_XP"))
		{
			OptionalParams += TEXT(" -SpecifiedArchitecture=_xp");
			bTargetPlatformCanUseCrashReporter = false;
		}
	}

	// Append any extra UAT flags specified for this platform flavor
	if (!PlatformInfo->UATCommandLine.IsEmpty())
	{
		OptionalParams += TEXT(" ");
		OptionalParams += PlatformInfo->UATCommandLine;
	}
	else
	{
		OptionalParams += TEXT(" -targetplatform=");
		OptionalParams += *PlatformInfo->TargetPlatformName.ToString();
	}

	// Get the target to build
	const FTargetInfo* Target = PackagingSettings->GetBuildTargetInfo();

	// Only build if the user elects to do so
	bool bBuild = false;
	if (PackagingSettings->Build == EProjectPackagingBuild::Always)
	{
		bBuild = true;
	}
	else if (PackagingSettings->Build == EProjectPackagingBuild::Never)
	{
		bBuild = false;
	}
	else if (PackagingSettings->Build == EProjectPackagingBuild::IfProjectHasCode)
	{
		bBuild = true;
		if (FApp::GetEngineIsPromotedBuild() && !bAssetNativizationEnabled)
		{
			FString BaseDir;

			// Get the target name
			FString TargetName;
			if (Target == nullptr)
			{
				TargetName = TEXT("UE4Game");
			}
			else
			{
				TargetName = Target->Name;
			}

			// Get the directory containing the receipt for this target, depending on whether the project needs to be built or not
			FString ProjectDir = FPaths::GetPath(FPaths::GetProjectFilePath());
			if (Target != nullptr && FPaths::IsUnderDirectory(Target->Path, ProjectDir))
			{
				UE_LOG(LogHotUpdateEditorTool, Log, TEXT("Selected target: %s"), *Target->Name);
				BaseDir = ProjectDir;
			}
			else
			{
				FText Reason;
				if (Platform->RequiresTempTarget(bProjectHasCode, ConfigurationInfo.Configuration, false, Reason))
				{
					UE_LOG(LogHotUpdateEditorTool, Log, TEXT("Project requires temp target (%s)"), *Reason.ToString());
					BaseDir = ProjectDir;
				}
				else
				{
					UE_LOG(LogHotUpdateEditorTool, Log, TEXT("Project does not require temp target"));
					BaseDir = FPaths::EngineDir();
				}
			}

			// Check if the receipt is for a matching promoted target
			FString PlatformName = Platform->GetPlatformInfo().UBTTargetId.ToString();

			// extern LAUNCHERSERVICES_API bool HasPromotedTarget(const TCHAR* BaseDir, const TCHAR* TargetName, const TCHAR* Platform, EBuildConfiguration Configuration, const TCHAR* Architecture);
			// if (HasPromotedTarget(*BaseDir, *TargetName, *PlatformName, ConfigurationInfo.Configuration, nullptr))
			// {
			// 	bBuild = false;
			// }
		}
	}
	else if (PackagingSettings->Build == EProjectPackagingBuild::IfEditorWasBuiltLocally)
	{
		bBuild = !FApp::GetEngineIsPromotedBuild();
	}
	if (bBuild)
	{
		OptionalParams += TEXT(" -build");
	}

	// Whether to include the crash reporter.
	if (PackagingSettings->IncludeCrashReporter && bTargetPlatformCanUseCrashReporter)
	{
		OptionalParams += TEXT(" -CrashReporter");
	}

	if (PackagingSettings->bBuildHttpChunkInstallData)
	{
		OptionalParams += FString::Printf(
			TEXT(" -manifests -createchunkinstall -chunkinstalldirectory=\"%s\" -chunkinstallversion=%s"),
			*(PackagingSettings->HttpChunkInstallDataDirectory.Path),
			*(PackagingSettings->HttpChunkInstallDataVersion));
	}

	int32 NumCookers = GetDefault<UEditorExperimentalSettings>()->MultiProcessCooking;
	if (NumCookers > 0)
	{
		OptionalParams += FString::Printf(TEXT(" -NumCookersToSpawn=%d"), NumCookers);
	}

	if (Target == nullptr)
	{
		OptionalParams += FString::Printf(TEXT(" -clientconfig=%s"), LexToString(ConfigurationInfo.Configuration));
	}
	else if (Target->Type == EBuildTargetType::Server)
	{
		OptionalParams += FString::Printf(
			TEXT(" -target=%s -serverconfig=%s"), *Target->Name, LexToString(ConfigurationInfo.Configuration));
	}
	else
	{
		OptionalParams += FString::Printf(
			TEXT(" -target=%s -clientconfig=%s"), *Target->Name, LexToString(ConfigurationInfo.Configuration));
	}

	FString ProjectPath = FPaths::IsProjectFilePathSet()
		                      ? FPaths::ConvertRelativePathToFull(FPaths::GetProjectFilePath())
		                      : FPaths::RootDir() / FApp::GetProjectName() / FApp::GetProjectName() + TEXT(".uproject");
	FString CommandLine = FString::Printf(
		TEXT(
			"-ScriptsForProject=\"%s\" BuildCookRun %s%s -nop4 -project=\"%s\" -cook -stage -archive -archivedirectory=\"%s\" -package -ue4exe=\"%s\" %s -utf8output"),
		*ProjectPath,
		GetUATCompilationFlags(),
		FApp::IsEngineInstalled() ? TEXT(" -installed") : TEXT(""),
		*ProjectPath,
		*PackagingSettings->StagingDirectory.Path,
		*FUnrealEdMisc::Get().GetExecutableForCommandlets(),
		*OptionalParams
	);

	IUATHelperModule::Get().CreateUatTask(CommandLine, PlatformInfo->DisplayName,
	                                      LOCTEXT("PackagingProjectTaskName", "Packaging project"),
	                                      LOCTEXT("PackagingTaskName", "Packaging"),
	                                      FEditorStyle::GetBrush(TEXT("MainFrame.PackageProject")),
	                                      Callback);
}

void FGenerateProjectTool::AddMessageLog(const FText& Text, const FText& Detail, const FString& TutorialLink,
                                         const FString& DocumentationLink)
{
	UE_LOG(LogHotUpdateEditorTool, Log, TEXT("%s"),
	       *(Text.ToString()+Detail.ToString()+TutorialLink+DocumentationLink));
	// TSharedRef<FTokenizedMessage> Message = FTokenizedMessage::Create(EMessageSeverity::Error);
	// Message->AddToken(FTextToken::Create(Text));
	// Message->AddToken(FTextToken::Create(Detail));
	// if (!TutorialLink.IsEmpty())
	// {
	// 	Message->AddToken(FTutorialToken::Create(TutorialLink));
	// }
	// if (!DocumentationLink.IsEmpty())
	// {
	// 	Message->AddToken(FDocumentationToken::Create(DocumentationLink));
	// }
	// FMessageLog MessageLog("PackagingResults");
	// MessageLog.AddMessage(Message);
	// MessageLog.Open();
}

TArray<FName> FGenerateProjectTool::GetAllPlatfrom()
{
	TArray<FName> Platforms;
	
	TArray<FName> AllPlatformSubMenus;
	const TArray<FString>& ConfidentalPlatforms = FDataDrivenPlatformInfoRegistry::GetConfidentialPlatforms();

	TArray<PlatformInfo::FVanillaPlatformEntry> VanillaPlatforms = PlatformInfo::BuildPlatformHierarchy(
		PlatformInfo::EPlatformFilter::All);
	if (!VanillaPlatforms.Num())
	{
		return Platforms;
	}

	VanillaPlatforms.Sort([](const PlatformInfo::FVanillaPlatformEntry& One,
	                         const PlatformInfo::FVanillaPlatformEntry& Two) -> bool
	{
		return One.PlatformInfo->DisplayName.CompareTo(Two.PlatformInfo->DisplayName) < 0;
	});

	IProjectTargetPlatformEditorModule& ProjectTargetPlatformEditorModule = FModuleManager::LoadModuleChecked<
		IProjectTargetPlatformEditorModule>("ProjectTargetPlatformEditor");
	EProjectType ProjectType = FGameProjectGenerationModule::Get().ProjectHasCodeFiles()
		                           ? EProjectType::Code
		                           : EProjectType::Content;

	// Build up a menu from the tree of platforms
	for (const PlatformInfo::FVanillaPlatformEntry& VanillaPlatform : VanillaPlatforms)
	{
		check(VanillaPlatform.PlatformInfo->IsVanilla());

		// Only care about game targets
		if (VanillaPlatform.PlatformInfo->PlatformType != EBuildTargetType::Game || !VanillaPlatform.PlatformInfo->
			bEnabledForUse || !FInstalledPlatformInfo::Get().CanDisplayPlatform(
				VanillaPlatform.PlatformInfo->BinaryFolderName, ProjectType))
		{
			continue;
		}

		// Make sure we're able to run this platform
		if (VanillaPlatform.PlatformInfo->bIsConfidential && !ConfidentalPlatforms.Contains(
			VanillaPlatform.PlatformInfo->IniPlatformName))
		{
			continue;
		}

		// Check if this platform has a submenu entry
		if (VanillaPlatform.PlatformInfo->PlatformSubMenu != NAME_None)
		{
			TArray<const PlatformInfo::FPlatformInfo*> SubMenuEntries;
			const FName& PlatformSubMenu = VanillaPlatform.PlatformInfo->PlatformSubMenu;

			// Check if we've already added this submenu
			if (AllPlatformSubMenus.Find(PlatformSubMenu) != INDEX_NONE)
				continue;
			AllPlatformSubMenus.Add(PlatformSubMenu);

			// Go through all vanilla platforms looking for matching submenus
			for (const PlatformInfo::FVanillaPlatformEntry& SubMenuVanillaPlatform : VanillaPlatforms)
			{
				const PlatformInfo::FPlatformInfo* PlatformInfo = SubMenuVanillaPlatform.PlatformInfo;

				if ((PlatformInfo->PlatformType == EBuildTargetType::Game) && (PlatformInfo->PlatformSubMenu ==
					PlatformSubMenu))
				{
					SubMenuEntries.Add(PlatformInfo);
				}
			}

			if (SubMenuEntries.Num())
			{
				for (const PlatformInfo::FPlatformInfo* SubPlatformInfo : SubMenuEntries)
				{
					if (SubPlatformInfo->PlatformType != EBuildTargetType::Game)
					{
						continue;
					}
					Platforms.Add(SubPlatformInfo->PlatformInfoName);
				}
			}
		}
		else if (VanillaPlatform.PlatformFlavors.Num())
		{
			for (const PlatformInfo::FPlatformInfo* SubPlatformInfo : VanillaPlatform.PlatformFlavors)
			{
				if (SubPlatformInfo->PlatformType != EBuildTargetType::Game)
				{
					continue;
				}
				Platforms.Add(SubPlatformInfo->PlatformInfoName);
			}
		}
		else
		{
			Platforms.Add(VanillaPlatform.PlatformInfo->PlatformInfoName);
		}
	}


	return Platforms;
}

#undef LOCTEXT_NAMESPACE
