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
			HandleStaticMesh(Cast<UStaticMesh>(AssetData.GetAsset()));
		else if (AssetData.AssetClass == USkeletalMesh::StaticClass()->GetFName())
			HandleSkeletalMesh(Cast<USkeletalMesh>(AssetData.GetAsset()));
		else if (AssetData.AssetClass == USkeleton::StaticClass()->GetFName())
			HandleSkeleton(Cast<USkeleton>(AssetData.GetAsset()));
		else if (AssetData.AssetClass == UAnimSequence::StaticClass()->GetFName())
			HandleAnimation(Cast<UAnimSequence>(AssetData.GetAsset()));
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
		ClassNames.Add(UAnimSequence::StaticClass()->GetFName());
	
	return ClassNames;
}

void UMeshAuditorBlueprintLibrary::HandleStaticMesh(const UStaticMesh* StaticMesh)
{
	UE_LOG(LogTemp, Log, TEXT("Static Mesh: %s --------------------------------"), *(StaticMesh->GetName()));
	const int32 NumLODs = StaticMesh->GetNumLODs();
	UE_LOG(LogTemp, Log, TEXT("Static Mesh: Number of LODs: %d"), NumLODs);

	for (int32 LODIndex = 0; LODIndex < NumLODs; ++LODIndex)
	{
		const FStaticMeshLODResources& LODResources = StaticMesh->GetRenderData()->LODResources[LODIndex];
		int32 TriangleCount = LODResources.GetNumTriangles();
		UE_LOG(LogTemp, Log, TEXT("  LOD %d: %d triangles"), LODIndex, TriangleCount);
	}

	const int32 NumMaterialSlots = StaticMesh->GetStaticMaterials().Num();
	UE_LOG(LogTemp, Log, TEXT("Static Mesh: Number of Material Slots: %d"), NumMaterialSlots);
}

void UMeshAuditorBlueprintLibrary::HandleSkeletalMesh(const USkeletalMesh* SkeletalMesh)
{
	UE_LOG(LogTemp, Log, TEXT("Skeletal Mesh: %s --------------------------------"), *(SkeletalMesh->GetName()));
	// Get the rendering data
	FSkeletalMeshRenderData* RenderData = SkeletalMesh->GetResourceForRendering();
	if (!RenderData)
	{
		UE_LOG(LogTemp, Warning, TEXT("No render data found for Skeletal Mesh: %s"), *SkeletalMesh->GetName());
		return;
	}

	// 1. Get the number of LODs
	const int32 NumLODs = RenderData->LODRenderData.Num();
	UE_LOG(LogTemp, Log, TEXT("Skeletal Mesh: Number of LODs: %d"), NumLODs);

	// 2. Get triangle count per LOD
	for (int32 LODIndex = 0; LODIndex < NumLODs; ++LODIndex)
	{
		const FSkeletalMeshLODRenderData& LODData = RenderData->LODRenderData[LODIndex];

		// Calculate the total triangle count for this LOD
		int32 TriangleCount = 0;
		for (const FSkelMeshRenderSection& Section : LODData.RenderSections)
		{
			TriangleCount += Section.NumTriangles;
		}

		UE_LOG(LogTemp, Log, TEXT("  LOD %d: %d triangles"), LODIndex, TriangleCount);
	}

	// 3. Get the number of material slots
	int32 NumMaterialSlots = SkeletalMesh->GetMaterials().Num();
	UE_LOG(LogTemp, Log, TEXT("Skeletal Mesh: Number of Material Slots: %d"), NumMaterialSlots);
}

void UMeshAuditorBlueprintLibrary::HandleSkeleton(const USkeleton* Skeleton)
{
	UE_LOG(LogTemp, Log, TEXT("Skeleton: %s --------------------------------"), *(Skeleton->GetName()));
	const int32 NumBones = Skeleton->GetReferenceSkeleton().GetNum();
	UE_LOG(LogTemp, Log, TEXT("Skeleton: Number of Bones: %d"), NumBones);
}

void UMeshAuditorBlueprintLibrary::HandleAnimation(const UAnimSequence* Animation)
{
	UE_LOG(LogTemp, Log, TEXT("Animation: %s --------------------------------"), *(Animation->GetName()));
	const int32 NumKeyFrames = Animation->GetNumberOfSampledKeys();
	UE_LOG(LogTemp, Log, TEXT("Animation Sequence: Number of Key Frames: %d"), NumKeyFrames);
}