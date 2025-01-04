// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshAuditorBlueprintLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"

void UMeshAuditorBlueprintLibrary::AuditAssets(FAuditSettings AuditSettings)
{
	// Get the Asset Registry module
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	const IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	
	// Specify search criteria
	FARFilter Filter;
	Filter.ClassNames = GetClassNamesFromIncludes(AuditSettings.Includes); // Type of Asset (SkeletalMesh, StaticMesh, etc)
	Filter.bRecursivePaths = true; // Search all subdirectories
	Filter.PackagePaths.Add(AuditSettings.RootDirectory); // Search under the root directory specified by the user

	// Retrieve asset data
	TArray<FAssetData> AssetDataList;
	AssetRegistry.GetAssets(Filter, AssetDataList);

	// Process the results
	for (const FAssetData& AssetData : AssetDataList)
	{
		if (AssetData.AssetClass == UStaticMesh::StaticClass()->GetFName())
			HandleStaticMesh();
		else if (AssetData.AssetClass == USkeletalMesh::StaticClass()->GetFName())
			HandleSkeletalMesh();
		else if (AssetData.AssetClass == USkeleton::StaticClass()->GetFName())
			HandleSkeleton();
		else if (AssetData.AssetClass == UAnimationAsset::StaticClass()->GetFName())
			HandleAnimation();
	}

}

TArray<FName> UMeshAuditorBlueprintLibrary::GetClassNamesFromIncludes(int32 Includes)
{
	TArray<FName> ClassNames = TArray<FName>();

	// Add class names based on user input
	if (EnumHasAnyFlags(static_cast<EAssetFlags>(Includes), EAssetFlags::StaticMesh))
		ClassNames.Add(UStaticMesh::StaticClass()->GetFName());
	if (EnumHasAnyFlags(static_cast<EAssetFlags>(Includes), EAssetFlags::SkeletalMesh))
		ClassNames.Add(USkeletalMesh::StaticClass()->GetFName());
	if (EnumHasAnyFlags(static_cast<EAssetFlags>(Includes), EAssetFlags::Skeleton))
		ClassNames.Add(USkeleton::StaticClass()->GetFName());
	if (EnumHasAnyFlags(static_cast<EAssetFlags>(Includes), EAssetFlags::Animation))
		ClassNames.Add(UAnimationAsset::StaticClass()->GetFName());
	
	return ClassNames;
}

void UMeshAuditorBlueprintLibrary::HandleStaticMesh()
{
	UE_LOG(LogTemp, Log, TEXT("FOUND STATIC MESH"))
}

void UMeshAuditorBlueprintLibrary::HandleSkeletalMesh()
{
	UE_LOG(LogTemp, Log, TEXT("FOUND SKELETAL MESH"))
}

void UMeshAuditorBlueprintLibrary::HandleSkeleton()
{
	UE_LOG(LogTemp, Log, TEXT("FOUND SKELETON"))
}

void UMeshAuditorBlueprintLibrary::HandleAnimation()
{
	UE_LOG(LogTemp, Log, TEXT("FOUND ANIMATION"))
}