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
	Drag = 4		UMETA(DisplayName = "Drag"),
};

USTRUCT(Atomic, BlueprintType)
struct FTile {
	GENERATED_BODY();
public:
	int8 bIsUsing : 1;
	FVector Location;
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class USceneComponent* HUDComp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UWidgetComponent* DragWidget;

protected:
#if WITH_EDITOR
	virtual void PostRegisterAllComponents() override;
#endif

#pragma region TileMap
private:
	int DefaultTileUnit = 100;
	
public:	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Data_Tile")
		TArray<FTile> TileArr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Data_Tile")
		TArray<FVector> BuildableLocations;


	/* Centimeter */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Option")
		int TileUnit = 100;

	/* X�� Ÿ�� ���� */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Option")
		int SizeX = 3;

	/* Y�� Ÿ�� ���� */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Option")
		int SizeY = 3;

	/* �浹ü�� Mesh ũ�⸦ ���� Ÿ�� ������ ���� */
	void SetTiles();
#pragma endregion

#pragma region Grid Option
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Option")
		bool bActiveGrid = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Grid")
		UMaterialInterface* BaseMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Grid")
		UMaterialInterface* GridMaterial;

	/* bActiveGrid Ȯ�� �� Ÿ���� ���͸���� ��ü */
	void OnGrid();
#pragma endregion

#pragma region HoveredDecal
public:
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data_Decal")
		class UDecalComponent* DragTileDecal;

	void SpawnHoveredDecal(FVector StartLocation, FVector CurrentLocation, EDecalType DecalType = EDecalType::Normal);
	void HideDecal(EDecalType DecalType = EDecalType::Normal);

private:
	UMaterialInterface* GetTargetDecalMI(UMaterialInterface* MI);
	UDecalComponent* GetTargetDecalComponent(UDecalComponent* DecalComp, UMaterialInterface* MI, FVector Size);
#pragma endregion

	
};
