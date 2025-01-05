// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshAuditorBlueprintLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"

DEFINE_LOG_CATEGORY(LogMeshAuditor);

void UMeshAuditorBlueprintLibrary::AuditAssets(FAuditSettings AuditSettings, EAuditType AuditType)
{
	if (AuditSettings.Includes ==  0)
	{
		UE_LOG(LogMeshAuditor, Log, TEXT("PLEASE SELECT INCLUDES FIRST BEFORE RUNNING AUDITOR"))
		return;
	}
	
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
	switch (AuditType)
	{
	case EAuditType::Individual:
		{
			UE_LOG(LogMeshAuditor, Display, TEXT("------------------INDIVIDUAL AUDIT RESULTS------------------"));
			for (const FAssetData& AssetData : AssetDataList)
			{
				UE_LOG(LogMeshAuditor, Display, TEXT("ASSET NAME: %s --------------------------"), *(AssetData.GetAsset()->GetName()));
				if (AssetData.AssetClass == UStaticMesh::StaticClass()->GetFName())
				{
					auto [NumLOD, NumTriangle, NumMaterialSlot] = HandleStaticMesh(Cast<UStaticMesh>(AssetData.GetAsset()));
					UE_LOG(LogMeshAuditor, Display, TEXT("ASSET TYPE: STATIC MESH"));
					UE_LOG(LogMeshAuditor, Display, TEXT("     Number of LODs: %llu"), NumLOD)
					for (int i = 0; i < NumTriangle.Num(); i++)
						UE_LOG(LogMeshAuditor, Display, TEXT("        Number of Triangles on LOD[%i]: %llu"), i, NumTriangle[i]);
					UE_LOG(LogMeshAuditor, Display, TEXT("     Number of Material Slots: %llu"), NumMaterialSlot);
				}
				else if (AssetData.AssetClass == USkeletalMesh::StaticClass()->GetFName())
				{
					auto [NumLOD, NumTriangle, NumMaterialSlot] = HandleSkeletalMesh(Cast<USkeletalMesh>(AssetData.GetAsset()));
					UE_LOG(LogMeshAuditor, Display, TEXT("ASSET TYPE: SKELETAL MESH"));
					UE_LOG(LogMeshAuditor, Display, TEXT("     Number of LODs: %llu"), NumLOD)
					for (int i = 0; i < NumTriangle.Num(); i++)
						UE_LOG(LogMeshAuditor, Display, TEXT("        Number of Triangles on LOD[%i]: %llu"), i, NumTriangle[i]);
					UE_LOG(LogMeshAuditor, Display, TEXT("     Number of Material Slots: %llu"), NumMaterialSlot);
				}
				else if (AssetData.AssetClass == USkeleton::StaticClass()->GetFName())
				{
					auto [NumLOD, NumBones] = HandleSkeleton(Cast<USkeleton>(AssetData.GetAsset()));
					UE_LOG(LogMeshAuditor, Display, TEXT("ASSET TYPE: SKELETON"));
					UE_LOG(LogMeshAuditor, Display, TEXT("     Number of LODs: %llu"), NumLOD);
					UE_LOG(LogMeshAuditor, Display, TEXT("     Number of Bones: %llu"), NumBones);
				}
				else if (AssetData.AssetClass == UAnimSequence::StaticClass()->GetFName())
				{
					auto [NumKeyFrames] = HandleAnimation(Cast<UAnimSequence>(AssetData.GetAsset()));
					UE_LOG(LogMeshAuditor, Display, TEXT("ASSET TYPE: ANIMATION"));
					UE_LOG(LogMeshAuditor, Display, TEXT("     Number of Key Frames: %llu"), NumKeyFrames);
				}
				UE_LOG(LogMeshAuditor, Display, TEXT("------------------------------------------------------------"));
			}
			UE_LOG(LogMeshAuditor, Display, TEXT("-------------------END OF AUDIT-----------------------------"));
		}
		break;
	case EAuditType::Total:
		{
			FMeshData StaticMeshData = {};
			FMeshData SkeletalMeshData = {};
			FSkeletonData SkeletonData = {};
			FAnimationData AnimationData = {};
			for (const FAssetData& AssetData : AssetDataList)
			{
				FMeshData CurrentStaticMeshData = {};
				FMeshData CurrentSkeletalMeshData = {};
				FSkeletonData CurrentSkeletonData = {};
				FAnimationData CurrentAnimationData = {};
				
				if (AssetData.AssetClass == UStaticMesh::StaticClass()->GetFName())
					CurrentStaticMeshData = HandleStaticMesh(Cast<UStaticMesh>(AssetData.GetAsset()));
				else if (AssetData.AssetClass == USkeletalMesh::StaticClass()->GetFName())
					CurrentSkeletalMeshData = HandleSkeletalMesh(Cast<USkeletalMesh>(AssetData.GetAsset()));
				else if (AssetData.AssetClass == USkeleton::StaticClass()->GetFName())
					CurrentSkeletonData = HandleSkeleton(Cast<USkeleton>(AssetData.GetAsset()));
				else if (AssetData.AssetClass == UAnimSequence::StaticClass()->GetFName())
					CurrentAnimationData = HandleAnimation(Cast<UAnimSequence>(AssetData.GetAsset()));
			
				// Static Mesh
				StaticMeshData.NumLOD += CurrentStaticMeshData.NumLOD;
				StaticMeshData.NumMaterialSlot += CurrentStaticMeshData.NumMaterialSlot;
				for (int i = 0; i < CurrentStaticMeshData.NumTriangle.Num(); i++)
					// StaticMeshData.NumTriangle[i] += CurrentStaticMeshData.NumTriangle[i];
				
				// Skeletal Mesh
				SkeletalMeshData.NumLOD += CurrentSkeletalMeshData.NumLOD;
				SkeletalMeshData.NumMaterialSlot += CurrentSkeletalMeshData.NumMaterialSlot;
				for (int i = 0; i < CurrentSkeletalMeshData.NumTriangle.Num(); i++)
					// SkeletalMeshData.NumTriangle[i] += CurrentSkeletalMeshData.NumTriangle[i];
			
				// Skeleton Data
				SkeletonData.NumLOD += CurrentSkeletonData.NumLOD;
				SkeletonData.NumBones += CurrentSkeletonData.NumBones;
			
				// Animation Data
				AnimationData.NumKeyFrames += CurrentAnimationData.NumKeyFrames;
			}
			
			UE_LOG(LogMeshAuditor, Display, TEXT("------------------TOTAL AUDIT RESULTS------------------"));
			if (EnumHasAnyFlags(static_cast<EAssetFlags>(AuditSettings.Includes), EAssetFlags::StaticMesh))
			{
				UE_LOG(LogMeshAuditor, Display, TEXT("STATIC MESH:"));
				UE_LOG(LogMeshAuditor, Display, TEXT("     Number of LODs: %llu"), StaticMeshData.NumLOD)
				for (int i = 0; i < StaticMeshData.NumTriangle.Num(); i++)
					UE_LOG(LogMeshAuditor, Display, TEXT("        Number of Triangles on LOD[%i]: %llu"), i, StaticMeshData.NumTriangle[i]);
				UE_LOG(LogMeshAuditor, Display, TEXT("     Number of Material Slots: %llu"), StaticMeshData.NumMaterialSlot);
			}
			if (EnumHasAnyFlags(static_cast<EAssetFlags>(AuditSettings.Includes), EAssetFlags::SkeletalMesh))
			{
				UE_LOG(LogMeshAuditor, Display, TEXT("SKELETAL MESH:"));
				UE_LOG(LogMeshAuditor, Display, TEXT("     Number of LODs: %llu"), SkeletalMeshData.NumLOD)
				for (int i = 0; i < SkeletalMeshData.NumTriangle.Num(); i++)
					UE_LOG(LogMeshAuditor, Display, TEXT("        Number of Triangles on LOD[%i]: %llu"), i, SkeletalMeshData.NumTriangle[i]);
				UE_LOG(LogMeshAuditor, Display, TEXT("     Number of Material Slots: %llu"), SkeletalMeshData.NumMaterialSlot);
			}
			if (EnumHasAnyFlags(static_cast<EAssetFlags>(AuditSettings.Includes), EAssetFlags::Skeleton))
			{
				UE_LOG(LogMeshAuditor, Display, TEXT("SKELETON:"));
				UE_LOG(LogMeshAuditor, Display, TEXT("     Number of LODs: %llu"), SkeletonData.NumLOD);
				UE_LOG(LogMeshAuditor, Display, TEXT("     Number of Bones: %llu"), SkeletonData.NumBones);
			}
			if (EnumHasAnyFlags(static_cast<EAssetFlags>(AuditSettings.Includes), EAssetFlags::Animation))
			{
				UE_LOG(LogMeshAuditor, Display, TEXT("ANIMATION:"));
				UE_LOG(LogMeshAuditor, Display, TEXT("     Number of Key Frames: %llu"), AnimationData.NumKeyFrames);
			}
			UE_LOG(LogMeshAuditor, Display, TEXT("-------------------END OF AUDIT-----------------------------"));
		}
		break;
	case EAuditType::Average:
		UE_LOG(LogMeshAuditor, Display, TEXT("NO AVAILABLE IMPLEMENTATION FOR AVERAGE"));
		break;
	}
	
	
}

int32 UMeshAuditorBlueprintLibrary::AddIncludeFlag(int32 CurrentFlags, EAssetFlags AssetFlag)
{
	return CurrentFlags | static_cast<int32>(AssetFlag);
}

int32 UMeshAuditorBlueprintLibrary::RemoveIncludeFlag(int32 CurrentFlags, EAssetFlags AssetFlag)
{
	return CurrentFlags & (~static_cast<int32>(AssetFlag));
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

FMeshData UMeshAuditorBlueprintLibrary::HandleStaticMesh(const UStaticMesh* StaticMesh)
{
	FMeshData MeshData = {};
	
	MeshData.NumLOD = StaticMesh->GetNumLODs();
	
	for (int32 LODIndex = 0; LODIndex < MeshData.NumLOD; LODIndex++)
	{
		const FStaticMeshLODResources& LODResources = StaticMesh->GetRenderData()->LODResources[LODIndex];
		MeshData.NumTriangle.Add(LODResources.GetNumTriangles());
	}

	MeshData.NumMaterialSlot = StaticMesh->GetStaticMaterials().Num();

	return MeshData;
}

FMeshData UMeshAuditorBlueprintLibrary::HandleSkeletalMesh(const USkeletalMesh* SkeletalMesh)
{
	FMeshData MeshData = {};
	// Get the rendering data
	FSkeletalMeshRenderData* RenderData = SkeletalMesh->GetResourceForRendering();
	if (!RenderData)
	{
		UE_LOG(LogMeshAuditor, Warning, TEXT("No render data found for Skeletal Mesh: %s"), *SkeletalMesh->GetName());
		return MeshData;
	}

	// 1. Get the number of LODs
	MeshData.NumLOD = RenderData->LODRenderData.Num();
	
	// 2. Get triangle count per LOD
	for (int32 LODIndex = 0; LODIndex < MeshData.NumLOD; LODIndex++)
	{
		const FSkeletalMeshLODRenderData& LODData = RenderData->LODRenderData[LODIndex];

		// Calculate the total triangle count for this LOD
		int32 TriangleCount = 0;
		for (const FSkelMeshRenderSection& Section : LODData.RenderSections)
		{
			TriangleCount += Section.NumTriangles;
		}

		MeshData.NumTriangle.Add(TriangleCount);
	}

	// 3. Get the number of material slots
	MeshData.NumMaterialSlot = SkeletalMesh->GetMaterials().Num();
	
	return MeshData;
}

FSkeletonData UMeshAuditorBlueprintLibrary::HandleSkeleton(const USkeleton* Skeleton)
{
	FSkeletonData SkeletonData = {};
	SkeletonData.NumBones =  Skeleton->GetReferenceSkeleton().GetNum();
	return SkeletonData;
}

FAnimationData UMeshAuditorBlueprintLibrary::HandleAnimation(const UAnimSequence* Animation)
{
	FAnimationData AnimData = {};
	AnimData.NumKeyFrames = Animation->GetNumberOfSampledKeys();
	return AnimData; 
}