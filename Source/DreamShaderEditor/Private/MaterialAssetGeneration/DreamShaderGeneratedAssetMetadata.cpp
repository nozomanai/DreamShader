#include "DreamShaderMaterialGeneratorPrivate.h"

#include "DreamShaderModule.h"

#include "FileHelpers.h"
#include "Misc/Crc.h"
#include "UObject/MetaData.h"
#include "UObject/Package.h"

namespace UE::DreamShader::Editor::Private
{
	namespace
	{
		FString GetSourceMetadataValue(UObject* Asset, const TCHAR* Key)
		{
			if (!Asset)
			{
				return FString();
			}

			UPackage* Package = Asset->GetOutermost();
			if (!Package)
			{
				return FString();
			}

			return Package->GetMetaData().GetValue(Asset, Key);
		}
	}

	FString BuildSourceHash(const FString& SourceText)
	{
		return FString::Printf(TEXT("%08x"), FCrc::StrCrc32(*SourceText));
	}

	bool IsGeneratedAssetSourceCurrent(UObject* Asset, const FString& SourceFilePath, const FString& SourceHash)
	{
		if (!Asset || SourceHash.IsEmpty())
		{
			return false;
		}

		const FString ExistingSourceFileRaw = GetSourceMetadataValue(Asset, TEXT("DreamShader.SourceFile"));
		if (ExistingSourceFileRaw.IsEmpty())
		{
			return false;
		}

		const FString ExistingSourceFile = UE::DreamShader::NormalizeSourceFilePath(ExistingSourceFileRaw);
		const FString ExistingSourceHash = GetSourceMetadataValue(Asset, TEXT("DreamShader.SourceHash"));

		return ExistingSourceFile.Equals(UE::DreamShader::NormalizeSourceFilePath(SourceFilePath), ESearchCase::IgnoreCase)
			&& ExistingSourceHash.Equals(SourceHash, ESearchCase::CaseSensitive);
	}

	void ApplySourceMetadata(UObject* Asset, const FString& SourceFilePath)
	{
		ApplySourceMetadata(Asset, SourceFilePath, FString());
	}

	void ApplySourceMetadata(UObject* Asset, const FString& SourceFilePath, const FString& SourceHash)
	{
		if (!Asset)
		{
			return;
		}

		UPackage* Package = Asset->GetOutermost();
		if (!Package)
		{
			return;
		}

		FMetaData& MetaData = Package->GetMetaData();
		MetaData.SetValue(Asset, TEXT("DreamShader.SourceFile"), *UE::DreamShader::NormalizeSourceFilePath(SourceFilePath));
		if (!SourceHash.IsEmpty())
		{
			MetaData.SetValue(Asset, TEXT("DreamShader.SourceHash"), *SourceHash);
			MetaData.SetValue(Asset, TEXT("DreamShader.GeneratedAtUtc"), *FDateTime::UtcNow().ToIso8601());
		}
	}

	bool SaveAssetPackage(UObject* Asset, FString& OutError)
	{
		check(Asset);

		TArray<UPackage*> PackagesToSave;
		PackagesToSave.Add(Asset->GetOutermost());
		if (!UEditorLoadingAndSavingUtils::SavePackages(PackagesToSave, true))
		{
			OutError = FString::Printf(TEXT("Generated DreamShader asset '%s' could not be saved."), *Asset->GetPathName());
			return false;
		}

		return true;
	}
}
