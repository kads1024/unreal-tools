// Fill out your copyright notice in the Description page of Project Settings.


#include "EUT_AssetAuditor.h"
#include "Async/TaskGraphInterfaces.h"
#include "AssetRegistry/AssetRegistryModule.h"

DEFINE_LOG_CATEGORY(LogAssetAuditor);

void UEUT_AssetAuditor::BeginExecution()
{
	Super::BeginExecution();

	AsyncTask(ENamedThreads::GameThread, [this]
	{

		SetTaskNotificationText(FText::FromString("Starting Task: Audit Assets..."));

	if (AuditSettings.Includes ==  0)
	{
		UE_LOG(LogAssetAuditor, Log, TEXT("PLEASE SELECT INCLUDES FIRST BEFORE RUNNING AUDITOR"))
		FinishExecutingTask();
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
	switch (AuditSettings.AuditType) // Can only be done on game thread
	{
	case EAuditType::Individual:
		{
			UE_LOG(LogAssetAuditor, Display, TEXT("------------------INDIVIDUAL AUDIT RESULTS------------------"));
			for (const FAssetData& AssetData : AssetDataList)
			{
				UE_LOG(LogAssetAuditor, Display, TEXT("ASSET NAME: %s --------------------------"), *(AssetData.GetAsset()->GetName()));
				if (AssetData.AssetClass == UStaticMesh::StaticClass()->GetFName())
				{
					auto [NumLOD, NumTriangle, NumMaterialSlot] = HandleStaticMesh(Cast<UStaticMesh>(AssetData.GetAsset()));
					UE_LOG(LogAssetAuditor, Display, TEXT("ASSET TYPE: STATIC MESH"));
					UE_LOG(LogAssetAuditor, Display, TEXT("     Number of LODs: %llu"), NumLOD)
					for (int i = 0; i < NumTriangle.Num(); i++)
						UE_LOG(LogAssetAuditor, Display, TEXT("        Number of Triangles on LOD[%i]: %llu"), i, NumTriangle[i]);
					UE_LOG(LogAssetAuditor, Display, TEXT("     Number of Material Slots: %llu"), NumMaterialSlot);
				}
				else if (AssetData.AssetClass == USkeletalMesh::StaticClass()->GetFName())
				{
					auto [NumLOD, NumTriangle, NumMaterialSlot] = HandleSkeletalMesh(Cast<USkeletalMesh>(AssetData.GetAsset()));
					UE_LOG(LogAssetAuditor, Display, TEXT("ASSET TYPE: SKELETAL MESH"));
					UE_LOG(LogAssetAuditor, Display, TEXT("     Number of LODs: %llu"), NumLOD)
					for (int i = 0; i < NumTriangle.Num(); i++)
						UE_LOG(LogAssetAuditor, Display, TEXT("        Number of Triangles on LOD[%i]: %llu"), i, NumTriangle[i]);
					UE_LOG(LogAssetAuditor, Display, TEXT("     Number of Material Slots: %llu"), NumMaterialSlot);
				}
				else if (AssetData.AssetClass == USkeleton::StaticClass()->GetFName())
				{
					auto [NumLOD, NumBones] = HandleSkeleton(Cast<USkeleton>(AssetData.GetAsset()));
					UE_LOG(LogAssetAuditor, Display, TEXT("ASSET TYPE: SKELETON"));
					UE_LOG(LogAssetAuditor, Display, TEXT("     Number of LODs: %llu"), NumLOD);
					UE_LOG(LogAssetAuditor, Display, TEXT("     Number of Bones: %llu"), NumBones);
				}
				else if (AssetData.AssetClass == UAnimSequence::StaticClass()->GetFName())
				{
					auto [NumKeyFrames] = HandleAnimation(Cast<UAnimSequence>(AssetData.GetAsset()));
					UE_LOG(LogAssetAuditor, Display, TEXT("ASSET TYPE: ANIMATION"));
					UE_LOG(LogAssetAuditor, Display, TEXT("     Number of Key Frames: %llu"), NumKeyFrames);
				}
				UE_LOG(LogAssetAuditor, Display, TEXT("------------------------------------------------------------"));
				if (WasCancelRequested())
				{
					UE_LOG(LogAssetAuditor, Display, TEXT("------------------AUDIT CANCELLED------------------"));
					FinishExecutingTask();
					return;
				}
			}
			UE_LOG(LogAssetAuditor, Display, TEXT("-------------------END OF AUDIT-----------------------------"));
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
				
				if (AssetData.AssetClass == UStaticMesh::StaticClass()->GetFName())
				{
					FMeshData Data = HandleStaticMesh(Cast<UStaticMesh>(AssetData.GetAsset()));

					// Static Mesh
					StaticMeshData.NumLOD += Data.NumLOD;
					StaticMeshData.NumMaterialSlot += Data.NumMaterialSlot;
					// for (int i = 0; i < Data.NumTriangle.Num(); i++)
						// StaticMeshData.NumTriangle[i] += Data.NumTriangle[i];
				}
				else if (AssetData.AssetClass == USkeletalMesh::StaticClass()->GetFName())
				{
					FMeshData Data = HandleSkeletalMesh(Cast<USkeletalMesh>(AssetData.GetAsset()));
					// Skeletal Mesh
					SkeletalMeshData.NumLOD += Data.NumLOD;
					SkeletalMeshData.NumMaterialSlot += Data.NumMaterialSlot;
					// for (int i = 0; i < CurrentSkeletalMeshData.NumTriangle.Num(); i++)
						// SkeletalMeshData.NumTriangle[i] += CurrentSkeletalMeshData.NumTriangle[i];
				}
					
				else if (AssetData.AssetClass == USkeleton::StaticClass()->GetFName())
				{
					auto [NumLOD, NumBones] = HandleSkeleton(Cast<USkeleton>(AssetData.GetAsset()));
					
					// Skeleton Data
					SkeletonData.NumLOD += NumLOD;
					SkeletonData.NumBones += NumBones;
				}
					
				else if (AssetData.AssetClass == UAnimSequence::StaticClass()->GetFName())
				{
					auto [NumKeyFrames] = HandleAnimation(Cast<UAnimSequence>(AssetData.GetAsset()));
					
					// Animation Data
					AnimationData.NumKeyFrames += NumKeyFrames;
				}
				if (WasCancelRequested())
				{
					UE_LOG(LogAssetAuditor, Display, TEXT("------------------AUDIT CANCELLED------------------"));
					FinishExecutingTask();
					return;
				}
			}
			
			UE_LOG(LogAssetAuditor, Display, TEXT("------------------TOTAL AUDIT RESULTS------------------"));
			if (EnumHasAnyFlags(static_cast<EAssetFlags>(AuditSettings.Includes), EAssetFlags::StaticMesh))
			{
				UE_LOG(LogAssetAuditor, Display, TEXT("STATIC MESH:"));
				UE_LOG(LogAssetAuditor, Display, TEXT("     Number of LODs: %llu"), StaticMeshData.NumLOD)
				for (int i = 0; i < StaticMeshData.NumTriangle.Num(); i++)
					UE_LOG(LogAssetAuditor, Display, TEXT("        Number of Triangles on LOD[%i]: %llu"), i, StaticMeshData.NumTriangle[i]);
				UE_LOG(LogAssetAuditor, Display, TEXT("     Number of Material Slots: %llu"), StaticMeshData.NumMaterialSlot);
			}
			if (EnumHasAnyFlags(static_cast<EAssetFlags>(AuditSettings.Includes), EAssetFlags::SkeletalMesh))
			{
				UE_LOG(LogAssetAuditor, Display, TEXT("SKELETAL MESH:"));
				UE_LOG(LogAssetAuditor, Display, TEXT("     Number of LODs: %llu"), SkeletalMeshData.NumLOD)
				for (int i = 0; i < SkeletalMeshData.NumTriangle.Num(); i++)
					UE_LOG(LogAssetAuditor, Display, TEXT("        Number of Triangles on LOD[%i]: %llu"), i, SkeletalMeshData.NumTriangle[i]);
				UE_LOG(LogAssetAuditor, Display, TEXT("     Number of Material Slots: %llu"), SkeletalMeshData.NumMaterialSlot);
			}
			if (EnumHasAnyFlags(static_cast<EAssetFlags>(AuditSettings.Includes), EAssetFlags::Skeleton))
			{
				UE_LOG(LogAssetAuditor, Display, TEXT("SKELETON:"));
				UE_LOG(LogAssetAuditor, Display, TEXT("     Number of LODs: %llu"), SkeletonData.NumLOD);
				UE_LOG(LogAssetAuditor, Display, TEXT("     Number of Bones: %llu"), SkeletonData.NumBones);
			}
			if (EnumHasAnyFlags(static_cast<EAssetFlags>(AuditSettings.Includes), EAssetFlags::Animation))
			{
				UE_LOG(LogAssetAuditor, Display, TEXT("ANIMATION:"));
				UE_LOG(LogAssetAuditor, Display, TEXT("     Number of Key Frames: %llu"), AnimationData.NumKeyFrames);
			}
			UE_LOG(LogAssetAuditor, Display, TEXT("-------------------END OF AUDIT-----------------------------"));
		}
		break;
	case EAuditType::Average:
		{
			FMeshData StaticMeshData = {};
			FMeshData SkeletalMeshData = {};
			FSkeletonData SkeletonData = {};
			FAnimationData AnimationData = {};

			int StaticCount = 0, SkeletalCount = 0, SkeletonCount = 0, AnimCount = 0;
			
			for (const FAssetData& AssetData : AssetDataList)
			{
				
				if (AssetData.AssetClass == UStaticMesh::StaticClass()->GetFName())
				{
					FMeshData Data = HandleStaticMesh(Cast<UStaticMesh>(AssetData.GetAsset()));

					// Static Mesh
					StaticMeshData.NumLOD += Data.NumLOD;
					StaticMeshData.NumMaterialSlot += Data.NumMaterialSlot;
					// for (int i = 0; i < Data.NumTriangle.Num(); i++)
						// StaticMeshData.NumTriangle[i] += Data.NumTriangle[i];
					StaticCount++;
				}
				else if (AssetData.AssetClass == USkeletalMesh::StaticClass()->GetFName())
				{
					FMeshData Data = HandleSkeletalMesh(Cast<USkeletalMesh>(AssetData.GetAsset()));
					// Skeletal Mesh
					SkeletalMeshData.NumLOD += Data.NumLOD;
					SkeletalMeshData.NumMaterialSlot += Data.NumMaterialSlot;
					// for (int i = 0; i < CurrentSkeletalMeshData.NumTriangle.Num(); i++)
						// SkeletalMeshData.NumTriangle[i] += CurrentSkeletalMeshData.NumTriangle[i];
					SkeletalCount++;
				}
					
				else if (AssetData.AssetClass == USkeleton::StaticClass()->GetFName())
				{
					auto [NumLOD, NumBones] = HandleSkeleton(Cast<USkeleton>(AssetData.GetAsset()));
					
					// Skeleton Data
					SkeletonData.NumLOD += NumLOD;
					SkeletonData.NumBones += NumBones;
					SkeletonCount++;
				}
					
				else if (AssetData.AssetClass == UAnimSequence::StaticClass()->GetFName())
				{
					auto [NumKeyFrames] = HandleAnimation(Cast<UAnimSequence>(AssetData.GetAsset()));
					
					// Animation Data
					AnimationData.NumKeyFrames += NumKeyFrames;
					AnimCount++;
				}
				if (WasCancelRequested())
				{
					UE_LOG(LogAssetAuditor, Display, TEXT("------------------AUDIT CANCELLED------------------"));
					FinishExecutingTask();
					return;
				}
			}
			
			UE_LOG(LogAssetAuditor, Display, TEXT("------------------AVERAGE AUDIT RESULTS------------------"));
			if (EnumHasAnyFlags(static_cast<EAssetFlags>(AuditSettings.Includes), EAssetFlags::StaticMesh))
			{
				UE_LOG(LogAssetAuditor, Display, TEXT("STATIC MESH:"));
				UE_LOG(LogAssetAuditor, Display, TEXT("     Number of LODs: %llu"), StaticMeshData.NumLOD / StaticCount)
				for (int i = 0; i < StaticMeshData.NumTriangle.Num(); i++)
					UE_LOG(LogAssetAuditor, Display, TEXT("        Number of Triangles on LOD[%i]: %llu"), i, StaticMeshData.NumTriangle[i] / StaticCount);
				UE_LOG(LogAssetAuditor, Display, TEXT("     Number of Material Slots: %llu"), StaticMeshData.NumMaterialSlot / StaticCount);
			}
			if (EnumHasAnyFlags(static_cast<EAssetFlags>(AuditSettings.Includes), EAssetFlags::SkeletalMesh))
			{
				UE_LOG(LogAssetAuditor, Display, TEXT("SKELETAL MESH:"));
				UE_LOG(LogAssetAuditor, Display, TEXT("     Number of LODs: %llu"), SkeletalMeshData.NumLOD / SkeletalCount)
				for (int i = 0; i < SkeletalMeshData.NumTriangle.Num(); i++)
					UE_LOG(LogAssetAuditor, Display, TEXT("        Number of Triangles on LOD[%i]: %llu"), i, SkeletalMeshData.NumTriangle[i] / SkeletalCount);
				UE_LOG(LogAssetAuditor, Display, TEXT("     Number of Material Slots: %llu"), SkeletalMeshData.NumMaterialSlot / SkeletalCount);
			}
			if (EnumHasAnyFlags(static_cast<EAssetFlags>(AuditSettings.Includes), EAssetFlags::Skeleton))
			{
				UE_LOG(LogAssetAuditor, Display, TEXT("SKELETON:"));
				UE_LOG(LogAssetAuditor, Display, TEXT("     Number of LODs: %llu"), SkeletonData.NumLOD / SkeletonCount);
				UE_LOG(LogAssetAuditor, Display, TEXT("     Number of Bones: %llu"), SkeletonData.NumBones / SkeletonCount);
			}
			if (EnumHasAnyFlags(static_cast<EAssetFlags>(AuditSettings.Includes), EAssetFlags::Animation))
			{
				UE_LOG(LogAssetAuditor, Display, TEXT("ANIMATION:"));
				UE_LOG(LogAssetAuditor, Display, TEXT("     Number of Key Frames: %llu"), AnimationData.NumKeyFrames / AnimCount);
			}
			UE_LOG(LogAssetAuditor, Display, TEXT("-------------------END OF AUDIT-----------------------------"));
			
		}
		break;
	}
		
	FinishExecutingTask();
	});
}


void UEUT_AssetAuditor::UpdateIncludeFlag(EAssetFlags InAssetFlag, bool bIsFlagIncluded)
{
	if (bIsFlagIncluded)
		AuditSettings.Includes |= static_cast<int32>(InAssetFlag);
	else
		AuditSettings.Includes &= ~static_cast<int32>(InAssetFlag);
}

void UEUT_AssetAuditor::SetAuditType(EAuditType InAuditType)
{
	AuditSettings.AuditType = InAuditType;
}

void UEUT_AssetAuditor::SetRootDirectory(FName InRootDirectory)
{
	AuditSettings.RootDirectory = InRootDirectory;
}

TArray<FName> UEUT_AssetAuditor::GetClassNamesFromIncludes(int32 Includes)
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

FMeshData UEUT_AssetAuditor::HandleStaticMesh(const UStaticMesh* StaticMesh)
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

FMeshData UEUT_AssetAuditor::HandleSkeletalMesh(const USkeletalMesh* SkeletalMesh)
{
	FMeshData MeshData = {};
	// Get the rendering data
	FSkeletalMeshRenderData* RenderData = SkeletalMesh->GetResourceForRendering();
	if (!RenderData)
	{
		UE_LOG(LogAssetAuditor, Warning, TEXT("No render data found for Skeletal Mesh: %s"), *SkeletalMesh->GetName());
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

FSkeletonData UEUT_AssetAuditor::HandleSkeleton(const USkeleton* Skeleton)
{
	FSkeletonData SkeletonData = {};
	SkeletonData.NumBones =  Skeleton->GetReferenceSkeleton().GetNum();
	return SkeletonData;
}

FAnimationData UEUT_AssetAuditor::HandleAnimation(const UAnimSequence* Animation)
{
	FAnimationData AnimData = {};
	AnimData.NumKeyFrames = Animation->GetNumberOfSampledKeys();
	return AnimData; 
}