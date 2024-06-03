#include "FReadWriteFileTest.h"

#include "Misc/FileHelper.h"

void FReadWriteFileTest::BeginTest()
{
	new FReadWriteFileTest();
}

FReadWriteFileTest::FReadWriteFileTest()
{
	FString TestStr = TEXT("Hello World! 你好，世界！𝌆");
	FFileHelper::SaveStringToFile(TestStr,TEXT("ReadWriteFile/AUX.txt"),FFileHelper::EEncodingOptions::AutoDetect,&IFileManager::Get(),FILEWRITE_NoReplaceExisting);
	UE_LOG(LogTemp,Log,TEXT("保存文件完成"));
	
	FString ReadStr;
	FFileHelper::LoadFileToString(ReadStr,TEXT("ReadWriteFile/Test Save File AutoDetect.txt"));
	UE_LOG(LogTemp,Log,TEXT("读取Test Save File AutoDetect.txt  ：%s"),*ReadStr);

	
}
