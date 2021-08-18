// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileMapEventManager.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FTileUnitDelegate_OneParam, int);


UCLASS()
class STEPPINGISLAND_API ATileMapEventManager : public AActor
{
	GENERATED_BODY()

// GetInstance
private:
	static ATileMapEventManager* Instance_;
public:
	static ATileMapEventManager* GetInstance();

public:	
	// Sets default values for this actor's properties
	ATileMapEventManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UFUNCTION()
	void PostChangedTileUnit(int Value);
	FTileUnitDelegate_OneParam F_ChangedTileUnitEvent;
};
