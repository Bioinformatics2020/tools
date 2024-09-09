// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HotUpdateEditorTool : ModuleRules
{
	public HotUpdateEditorTool(ReadOnlyTargetRules Target) : base(Target)
	{
		if (Target.Type != TargetType.Editor)
		{
			throw new BuildException("Unable to instantiate UnrealEd module for non-editor targets.");
		}

		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[]
			{
				// ... add public include paths required here ...
			}
		);


		PrivateIncludePaths.AddRange(
			new string[]
			{
				// ... add other private include paths required here ...
			}
		);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "Slate",
				// ... add other public dependencies that you statically link with here ...
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"CoreUObject",
				"InputCore",
				"UnrealEd",
				"Engine",
				"ToolMenus",
				"Slate",
				"SlateCore",
				"HotUpdateTool",
				"GameProjectGeneration",
				"UATHelper",
				"MainFrame",
				"ProjectTargetPlatformEditor",
				"DerivedDataCache",
				"DesktopPlatform",
				"EDITORSTYLE",
				"ENGINESETTINGS",
				"TARGETPLATFORM",
				// ... add private dependencies that you statically link with here ...	
			}
		);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}