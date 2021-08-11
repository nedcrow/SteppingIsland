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

	// Tile structs
	TileArr.Empty();

	float halfTileMapSizeX = SizeX * TileUnit * 0.5f;
	float halfTileMapSizeY = SizeY * TileUnit * 0.5f;
	float halfTileUnit = TileUnit * 0.5f;
	TArray<float> TileLocationArrX;
	TArray<float> TileLocationArrY;
	for (int i = 0; i < halfTileMapSizeX; i++) { TileLocationArrX.Add(-halfTileMapSizeX + halfTileUnit + (TileUnit * i)); }
	for (int i = 0; i < halfTileMapSizeY; i++) { TileLocationArrY.Add(-halfTileMapSizeY + halfTileUnit + (TileUnit * i)); }
	for (int i = 0; i < SizeX; i++) {
		for (int j = 0; j < SizeY; j++) {		
			FTile tile;
			tile.bIsUsing = 1;
			tile.Location = FVector(TileLocationArrX[i], TileLocationArrY[j], SM->GetRelativeLocation().Z);
			TileArr.Add(tile);
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

void ATileMap::SpawnHoveredDecal(FVector StartLocation, FVector CurrentLocation, EDecalType DecalType)
{
	// Material
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
	case EDecalType::Drag:
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

	// Size & Location
	FVector decalSize = FVector(TileUnit * 0.5f, TileUnit * 0.5f, TileUnit);
	FVector decalDragSize = FVector::OneVector;
	FVector decalLocation = FVector::ZeroVector;
	float halfTileMapSizeX = SizeX * TileUnit * 0.5f;
	float halfTileMapSizeY = SizeY * TileUnit * 0.5f;
	
	bool isInTileMap = StartLocation.X < halfTileMapSizeX&& StartLocation.X > -halfTileMapSizeX &&
		StartLocation.Y < halfTileMapSizeY&& StartLocation.Y > -halfTileMapSizeY;
	if (isInTileMap) {
		// DecalStartLocation
		FVector decalStartLocation = FVector::ZeroVector;
		float lastDist_A = SizeX * SizeY * TileUnit;
		for (auto tile : TileArr) {
			float currentDist_A = FVector::Dist(tile.Location, StartLocation);
			if (currentDist_A < lastDist_A) {
				lastDist_A = currentDist_A;
				decalStartLocation = tile.Location;
			}
		}

		// DecalCurrentLocation
		FVector decalCurrentLocation = FVector::ZeroVector;
		float lastDist_B = SizeX * SizeY * TileUnit;
		for (auto tile : TileArr) {
			float currentDist_B = FVector::Dist(tile.Location, CurrentLocation);
			if (currentDist_B < lastDist_B) {
				lastDist_B = currentDist_B;
				decalCurrentLocation = tile.Location;
			}
		}

		// Decal 크기가 1 x 1 칸 초과면 사이즈 및 위치 연산
		bool isSame = FVector::Dist(decalStartLocation, decalCurrentLocation) == 0;
		if (isSame) {
			decalLocation = decalCurrentLocation;
		}
		else {
			float distPerUnit_X = FMath::Abs(decalStartLocation.X - decalCurrentLocation.X) / TileUnit + 1;
			float distPerUnit_Y = FMath::Abs(decalStartLocation.Y - decalCurrentLocation.Y) / TileUnit + 1;
			decalDragSize = FVector(
				distPerUnit_X,
				distPerUnit_Y,
				1.f
			);
			decalLocation = FVector(
				(decalStartLocation.X + decalCurrentLocation.X) * 0.5f,
				(decalStartLocation.Y + decalCurrentLocation.Y) * 0.5f,
				decalCurrentLocation.Z
			);
		}
	}

	// DecalComponent, 없으면 생성
	UDecalComponent* currentDecalComp;

	if (DecalType == EDecalType::Drag) {
		if (DragTileDecal == nullptr) {
			DragTileDecal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), decalMI, decalSize, SM->GetRelativeLocation(), FRotator().ZeroRotator, 0.0f);
		}
		currentDecalComp = DragTileDecal;		
	}
	else {
		if (CursorTileDecal == nullptr) {
			CursorTileDecal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), decalMI, decalSize, SM->GetRelativeLocation(), FRotator().ZeroRotator, 0.0f);
		}
		currentDecalComp = CursorTileDecal;
	}

	// Decal 배치
	bool canDisplayDecal = decalMI != nullptr && 
		(currentDecalComp != nullptr && currentDecalComp->GetRelativeLocation().X != decalLocation.X || currentDecalComp->GetRelativeLocation().Y != decalLocation.Y);
	if (canDisplayDecal) {
		currentDecalComp->Activate(true);
		if (DecalType == EDecalType::Drag) {
			currentDecalComp->SetWorldScale3D(decalDragSize);
		}
		currentDecalComp->SetRelativeLocation(decalLocation);
	}
}

