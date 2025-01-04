// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MeshAuditorBlueprintLibrary.generated.h"

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

USTRUCT(BlueprintType)
struct FAuditSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuditSettings)
	FName RootDirectory;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AuditSettings, meta=(Bitmask, BitmaskEnum=EAssetFlags))
	int32 Includes;
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
	static void AuditAssets(FAuditSettings AuditSettings);

private:
	UFUNCTION()
	static TArray<FName> GetClassNamesFromIncludes(int32 Includes);

	UFUNCTION()
	static void HandleStaticMesh();

	UFUNCTION()
	static void HandleSkeletalMesh();

	UFUNCTION()
	static void HandleSkeleton();

	UFUNCTION()
	static void HandleAnimation();
};
