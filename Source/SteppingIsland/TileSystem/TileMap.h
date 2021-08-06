// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileMap.generated.h"

UENUM(BlueprintType)
enum class EDecalType : uint8
{
	Normal = 0			UMETA(DisplayName = "Normal"),
	Build = 1			UMETA(DisplayName = "Build"),
	NotBuild = 2		UMETA(DisplayName = "NotBuild"),
	EffectArea = 3		UMETA(DisplayName = "EffectArea"),
};

UCLASS()
class STEPPINGISLAND_API ATileMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileMap();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UBoxComponent* Box;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UStaticMeshComponent* SM;

protected:
#if WITH_EDITOR
	virtual void PostRegisterAllComponents() override;
#endif
	virtual void BeginPlay() override;

public:	
#pragma region TileMap
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Data_Locations")
		TArray<FVector> TileLocations;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Data_Locations")
		TArray<FVector> BuildableLocations;

	/* Centimeter */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Option")
		int TileUnit = 100;

	/* XÃà Å¸ÀÏ °¹¼ö */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Option")
		int SizeX = 3;

	/* YÃà Å¸ÀÏ °¹¼ö */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Option")
		int SizeY = 3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Option")
		float TileScale = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Option")
		bool bActiveGrid = false;

	void SetTiles();
#pragma endregion

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Grid")
		UMaterialInterface* BaseMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Grid")
		UMaterialInterface* GridMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Decal")
		UMaterialInterface* NormalDecalMI;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Decal")
		UMaterialInterface* BuildDecalMI;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Decal")
		UMaterialInterface* NotBuildDecalMI;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Decal")
		UMaterialInterface* EffectAreaDecalMI;

	class UDecalComponent* CursorTileDecal;

	void OnGrid();
	void SpawnHoveredDecal(FVector Location, EDecalType DecalType = EDecalType::Normal);	
};
