// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityTask.h"
#include "EUT_AssetAuditor.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAssetAuditor, Log, All);

UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class EAssetFlags : uint8
{
	None = 0 UMETA(Hidden),
	StaticMesh = 1, // 0x00000001
	SkeletalMesh = 2 , // 0x00000010
	Skeleton = 4, // 0xb00000100
	Animation = 8, // 0x00001000
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuditSettings)
	EAuditType AuditType;
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
class UEUT_AssetAuditor : public UEditorUtilityTask
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "MeshAuditingTool")
	void UpdateIncludeFlag(EAssetFlags InAssetFlag, bool bIsFlagIncluded);

	UFUNCTION(BlueprintCallable, Category = "MeshAuditingTool")
	void SetAuditType(EAuditType InAuditType);

	UFUNCTION(BlueprintCallable, Category = "MeshAuditingTool")
	void SetRootDirectory(FName InRootDirectory);
	
protected:
	virtual void BeginExecution() override;

private:
	UPROPERTY()
	FAuditSettings AuditSettings;
	
private:
	UFUNCTION()
	static TArray<FName> GetClassNamesFromIncludes(int32 Includes);

	UFUNCTION()
	FMeshData HandleStaticMesh(const UStaticMesh* StaticMesh);

	UFUNCTION()
	FMeshData HandleSkeletalMesh(const USkeletalMesh* SkeletalMesh);

	UFUNCTION()
	FSkeletonData HandleSkeleton(const USkeleton* Skeleton);

	UFUNCTION()
	FAnimationData HandleAnimation(const UAnimSequence* Animation);
};
