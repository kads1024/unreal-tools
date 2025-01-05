// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MeshAuditorBlueprintLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMeshAuditor, Log, All);

UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class EAssetFlags : uint8
{
	None = 0 UMETA(Hidden),
	StaticMesh = 1, // 0b00000001
	SkeletalMesh = 2 , // 0b00000010
	Skeleton = 4, // 0b00000100
	Animation = 8, // 0b00001000
};
ENUM_CLASS_FLAGS(EAssetFlags)

UENUM(BlueprintType)
enum class EAuditType : uint8
{
	Individual = 0 UMETA(DisplayName = "Individual"),
	Total = 1  UMETA(DisplayName = "Total"),
	Average = 2     UMETA(DisplayName = "Average"),
};

USTRUCT(BlueprintType)
struct FAuditSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuditSettings)
	FName RootDirectory;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuditSettings, meta=(Bitmask, BitmaskEnum=EAssetFlags))
	int32 Includes;
};

USTRUCT()
struct FMeshData
{
	GENERATED_BODY()

	UPROPERTY()
	uint64 NumLOD;
	
	UPROPERTY()
	TArray<uint64> NumTriangle;

	UPROPERTY()
	uint64 NumMaterialSlot;
};

USTRUCT()
struct FSkeletonData
{
	GENERATED_BODY()

	UPROPERTY()
	uint64 NumLOD;
	
	UPROPERTY()
	uint64 NumBones;
	
};

USTRUCT()
struct FAnimationData
{
	GENERATED_BODY()

	UPROPERTY()
	uint64 NumKeyFrames;
};
/**
 * 
 */
UCLASS()
class UMeshAuditorBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "MeshAuditingTool")
	static void AuditAssets(FAuditSettings AuditSettings, EAuditType AuditType);
	
	UFUNCTION(BlueprintPure, Category = "MeshAuditingTool")
	static int32 AddIncludeFlag(int32 CurrentFlags, EAssetFlags AssetFlag);

	UFUNCTION(BlueprintPure, Category = "MeshAuditingTool")
	static int32 RemoveIncludeFlag(int32 CurrentFlags, EAssetFlags AssetFlag);
	
private:
	
	UFUNCTION()
	static TArray<FName> GetClassNamesFromIncludes(int32 Includes);

	UFUNCTION()
	static FMeshData HandleStaticMesh(const UStaticMesh* StaticMesh);

	UFUNCTION()
	static FMeshData HandleSkeletalMesh(const USkeletalMesh* SkeletalMesh);

	UFUNCTION()
	static FSkeletonData HandleSkeleton(const USkeleton* Skeleton);

	UFUNCTION()
	static FAnimationData HandleAnimation(const UAnimSequence* Animation);
};
