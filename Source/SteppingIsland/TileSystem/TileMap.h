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

#pragma region TileMap
private:
	int DefaultTileUnit = 100;
	
public:	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Data_Tile")
		TArray<uint8> TileActiveArr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Data_Tile")
		TArray<FVector> TileLocations;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Data_Tile")
		TArray<FVector> BuildableLocations;


	/* Centimeter */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Option")
		int TileUnit = 100;

	/* X축 타일 갯수 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Option")
		int SizeX = 3;

	/* Y축 타일 갯수 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Option")
		int SizeY = 3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Option")
		bool bActiveGrid = false;

	/* 충돌체와 Mesh 크기를 맵의 타일 갯수에 맞춤 */
	void SetTiles();
#pragma endregion

public:
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Decal")
	class UDecalComponent* CursorTileDecal;

	/* bActiveGrid 확인 후 타당한 메터리얼로 교체 */
	void OnGrid();
	void SpawnHoveredDecal(FVector Location, EDecalType DecalType = EDecalType::Normal);	
};
