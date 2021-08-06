// Fill out your copyright notice in the Description page of Project Settings.


#include "TileMap.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATileMap::ATileMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;

	SM = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM"));
	SM->SetupAttachment(Box);
	SM->ComponentTags.Add("Tile");
	Tags.Add("TileMap");
}

#if WITH_EDITOR
void ATileMap::PostRegisterAllComponents()
{
	SetTiles();
	OnGrid();
}
#endif

void ATileMap::SetTiles()
{
	// Tilemap Mesh
	float halfExtent = TileUnit * 0.5f;
	float tileScale = FMath::Floor(((float)TileUnit / DefaultTileUnit) * 100.f) / 100.f;
	Box->SetBoxExtent(FVector(SizeX * halfExtent, SizeY * halfExtent, 0.1f));
	SM->SetRelativeScale3D(FVector(SizeX * tileScale, SizeY * tileScale, 0.1f));

	// Tile locations
	float halfTileMapSizeX = SizeX * TileUnit * 0.5f;
	float halfTileMapSizeY = SizeY * TileUnit * 0.5f;
	float halfTileUnit = TileUnit * 0.5f;
	TArray<float> TileLocationArrX;
	TArray<float> TileLocationArrY;
	for (int i = 0; i < halfTileMapSizeX; i++) { TileLocationArrX.Add(-halfTileMapSizeX + halfTileUnit + (TileUnit * i)); }
	for (int i = 0; i < halfTileMapSizeY; i++) { TileLocationArrY.Add(-halfTileMapSizeY + halfTileUnit + (TileUnit * i)); }
	for (int i = 0; i < SizeX; i++) {
		for (int j = 0; j < SizeY; j++) {			
			TileActiveArr.Push(1);
			TileLocations.Push(FVector(TileLocationArrX[i], TileLocationArrY[j], SM->GetRelativeLocation().Z));
		}
	}
}

void ATileMap::OnGrid()
{
	if (bActiveGrid == true) {
		if (GridMaterial != nullptr) SM->SetMaterial(0, GridMaterial);
	}
	else {
		if (BaseMaterial != nullptr) SM->SetMaterial(0, BaseMaterial);
	}
}

void ATileMap::SpawnHoveredDecal(FVector Location, EDecalType DecalType)
{
	FVector decalLocation = FVector::ZeroVector;
	float halfTileMapSizeX = SizeX * TileUnit * 0.5f;
	float halfTileMapSizeY = SizeY * TileUnit * 0.5f;

	bool isInTileMap = Location.X < halfTileMapSizeX&& Location.X > -halfTileMapSizeX &&
		Location.Y < halfTileMapSizeY&& Location.Y > -halfTileMapSizeY;
	if (isInTileMap) {
		float lastDist = SizeX * SizeY * TileUnit;
		for (FVector tileLocation : TileLocations) {
			float currentDist = FVector::Dist(tileLocation, Location);
			if(currentDist < lastDist) {
				lastDist = currentDist;	
				decalLocation = tileLocation;
			}
		}
	}

	UMaterialInterface* decalMI = nullptr;
	switch (DecalType)
	{
	case EDecalType::Normal:
		if (NormalDecalMI == nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("!!!- Null : NormalDecalMI -!!!"));
		}
		else {
			decalMI = NormalDecalMI;
		}
		break;
	case EDecalType::Build:
		if (NormalDecalMI == nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("!!!- Null : NormalDecalMI -!!!"));
		}
		else {
			decalMI = NormalDecalMI;
		}
		break;
	case EDecalType::NotBuild:
		if (NormalDecalMI == nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("!!!- Null : NormalDecalMI -!!!"));
		}
		else {
			decalMI = NormalDecalMI;
		}
		break;
	case EDecalType::EffectArea:
		if (NormalDecalMI == nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("!!!- Null : NormalDecalMI -!!!"));
		}
		else {
			decalMI = NormalDecalMI;
		}
		break;
	default:
		break;
	}

	// 커서 위치 확인 용 Decal 없으면 생성
	if (CursorTileDecal == nullptr) {
		CursorTileDecal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), decalMI, FVector(TileUnit*0.5f, TileUnit*0.5f, TileUnit), SM->GetRelativeLocation(), FRotator().ZeroRotator, 0.0f);
	}

	// Decal 배치
	bool canSpawnDecal = decalMI != nullptr && 
		(CursorTileDecal->GetRelativeLocation().X != decalLocation.X || CursorTileDecal->GetRelativeLocation().Y != decalLocation.Y);
	if (canSpawnDecal) {
		CursorTileDecal->Activate(true);
		CursorTileDecal->SetRelativeLocation(decalLocation);
	}
}

