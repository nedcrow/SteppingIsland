// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingBase.h"
#include "../TileMap.h"
#include "../TileMapEventManager.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABuildingBase::ABuildingBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SM = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM"));
	SM->SetupAttachment(Box);
}

#if WITH_EDITOR
void ABuildingBase::PostRegisterAllComponents()
{
	ATileMap* TileMap = Cast<ATileMap>(UGameplayStatics::GetActorOfClass(GetWorld(), ATileMap::StaticClass()));
	if (TileMap) {		
		SetBuildingScale(TileMap->TileUnit);
	}
}
#endif

void ABuildingBase::BeginPlay() {
	/*ATileMapEventManager* eventM = ATileMapEventManager::GetInstance();
	eventM->F_ChangedTileUnitEvent.AddUFunction(this, TEXT("SetBuildingScale"));*/
}

void ABuildingBase::SetBuildingScale(int TileUnit)
{	
	// 기본 크기는 Int 고정
	Size = FVector(
		FMath::Floor(Size.X),
		FMath::Floor(Size.Y),
		FMath::Floor(Size.Z)
	);

	// Box 크기
	Box->SetBoxExtent(Size * TileUnit * 0.5f);

	// 타일 크기에 맞춤
	float unitPercentage = TileUnit / BuildingUnit;
	SM->SetRelativeScale3D(Size * unitPercentage);
}
