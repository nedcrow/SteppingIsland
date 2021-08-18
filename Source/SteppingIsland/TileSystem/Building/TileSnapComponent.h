// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../TileMap.h"
#include "Components/SceneComponent.h"
#include "TileSnapComponent.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FSnapableLocation
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int IndexX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int IndexY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector TileLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DistanceWithThis;
};

USTRUCT(Atomic, BlueprintType)
struct FCheckTileMatrixProp
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int LayerIndex = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int StartX;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int EndX;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int StartY;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int EndY;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STEPPINGISLAND_API UTileSnapComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UTileSnapComponent();

protected:
#if WITH_EDITOR
	virtual void PostEditComponentMove(bool bFinished) override;
#endif

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int TargetTileMapLayer = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FCheckTileMatrixProp InfoOfCheckedTileMatrixProp;

	/*
	* 스냅 대상 : 이 컴포넌트를 가진 Actor
	* 스냅 위치 : TileManager 매트릭스 중 가장 가까운 Actor
	*/
	void SnapToTileMap(bool bFinished);
	FSnapableLocation GetNeedSnapableLocation(TArray<FTile> TileArr);
	
};
