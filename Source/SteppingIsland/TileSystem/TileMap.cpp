// Fill out your copyright notice in the Description page of Project Settings.


#include "TileMap.h"
#include "TileMapEventManager.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
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

	HUDComp = CreateDefaultSubobject<USceneComponent>(TEXT("HUDComp"));
	HUDComp->SetupAttachment(Box);
	HUDComp->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));

	DragWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DragWidget"));
	DragWidget->SetupAttachment(HUDComp);	
	DragWidget->SetVisibility(false);
}

#if WITH_EDITOR
void ATileMap::PostRegisterAllComponents()
{
	CreateTileMaps();
	OnGrid();
}
#endif

void ATileMap::CreateTileMaps()
{
	// Tilemap Mesh
	float halfExtent = TileUnit * 0.5f;
	float tileScale = FMath::Floor(((float)TileUnit / DefaultTileUnit) * 100.f) / 100.f;
	Box->SetBoxExtent(FVector(SizeX * halfExtent, SizeY * halfExtent, 0.1f));
	SM->SetRelativeScale3D(FVector(SizeX * tileScale, SizeY * tileScale, 0.1f));

	// TileMapLayers
	TileMapLayers.Empty();
	for (int i = 0; i < MaxTileMapLayer; i++) {
		FTileMapLayer tileMap = CreateTileMap();
		tileMap.Index = i;
		TileMapLayers.Add(tileMap);
	}
}

FTileMapLayer ATileMap::CreateTileMap()
{
	FTileMapLayer tileMap;

	float halfTileMapSizeX = GetHalfTileMapSize().X;
	float halfTileMapSizeY = GetHalfTileMapSize().Y;
	float halfTileUnit = TileUnit * 0.5f;
	TArray<float> TileLocationArrX;
	TArray<float> TileLocationArrY;
	for (int i = 0; i < halfTileMapSizeX; i++) { TileLocationArrX.Add(-halfTileMapSizeX + halfTileUnit + (TileUnit * i)); }
	for (int i = 0; i < halfTileMapSizeY; i++) { TileLocationArrY.Add(-halfTileMapSizeY + halfTileUnit + (TileUnit * i)); }
	for (int i = 0; i < SizeX; i++) {
		FTileRaw tileRawY;
		for (int j = 0; j < SizeY; j++) {
			FTile tile;
			tile.IndexX = i;
			tile.IndexY = j;
			tile.bIsUsing = 1;
			tile.Location = FVector(TileLocationArrX[i], TileLocationArrY[j], SM->GetRelativeLocation().Z);
			tileRawY.TileArrY.Add(tile);
		}
		tileMap.TileMatrix.Add(tileRawY);
	}
	return tileMap;
}

FVector2D ATileMap::GetHalfTileMapSize()
{
	return FVector2D(SizeX * TileUnit * 0.5f, SizeY * TileUnit * 0.5f);
}

TArray<FTile> ATileMap::GetTileArr(int LayerIndex)
{
	if (TileMapLayers.Num() > LayerIndex) {
		TArray<FTile> tileArr;
		for (auto raw : TileMapLayers[LayerIndex].TileMatrix) {
			tileArr.Append(raw.TileArrY);
		}
		return tileArr;
	}
	else {
		return TArray<FTile>();
	}
}

void ATileMap::CheckTileMatrix(int LayerIndex, bool IsUsing, int StartX, int EndX, int StartY, int EndY)
{

	for (int i = 0; i < EndX - StartX; i++) {
		for (int j = 0; j < EndY - StartY; j++) {
			int indexX = StartX + i;
			int indexY = StartY + j;
			TileMapLayers[LayerIndex].TileMatrix[indexX].TileArrY[indexY].bIsUsing = true;
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
	// Size & Location
	FVector decalSize = FVector(TileUnit * 0.5f, TileUnit * 0.5f, TileUnit);
	FVector decalDragSize = FVector::OneVector;
	FVector decalLocation = FVector::ZeroVector;
	float halfTileMapSizeX = SizeX * TileUnit * 0.5f;
	float halfTileMapSizeY = SizeY * TileUnit * 0.5f;
	
	bool isInTileMap = StartLocation.X < halfTileMapSizeX&& StartLocation.X > -halfTileMapSizeX &&
		StartLocation.Y < halfTileMapSizeY&& StartLocation.Y > -halfTileMapSizeY;
	if (isInTileMap) {
		TArray<FTile> tileArr = GetTileArr(0);
		// DecalStartLocation
		FVector decalStartLocation = FVector::ZeroVector;
		float lastDist_A = SizeX * SizeY * TileUnit;
		for (auto tile : tileArr) {
			float currentDist_A = FVector::Dist(tile.Location, StartLocation);
			if (currentDist_A < lastDist_A) {
				lastDist_A = currentDist_A;
				decalStartLocation = tile.Location;
			}
		}

		// DecalCurrentLocation
		FVector decalCurrentLocation = FVector::ZeroVector;
		float lastDist_B = SizeX * SizeY * TileUnit;
		for (auto tile : tileArr) {
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

	// Material 및 DecalComponent 준비
	UMaterialInterface* decalMI = nullptr;
	UDecalComponent* currentDecalComp = nullptr;
	switch (DecalType)
	{
	case EDecalType::Normal:
		decalMI = GetTargetDecalMI(NormalDecalMI);
		currentDecalComp = CursorTileDecal = GetTargetDecalComponent(CursorTileDecal, decalMI, decalSize);
		break;
	case EDecalType::Build:
		decalMI = GetTargetDecalMI(NormalDecalMI);
		break;
	case EDecalType::NotBuild:
		decalMI = GetTargetDecalMI(NormalDecalMI);
		break;
	case EDecalType::EffectArea:
		decalMI = GetTargetDecalMI(NormalDecalMI);
		break;
	case EDecalType::Drag:
		decalMI = GetTargetDecalMI(NormalDecalMI);
		currentDecalComp = DragTileDecal = GetTargetDecalComponent(DragTileDecal, decalMI, decalSize);
		currentDecalComp->SetRelativeScale3D(decalDragSize);

		// HUD 배치
		if (DragWidget && DragWidget != nullptr) {
			if(!DragWidget->IsVisible())DragWidget->SetVisibility(true);
			DragWidget->SetDrawSize(FVector2D(decalDragSize.Y * TileUnit, decalDragSize.X * TileUnit));
			HUDComp->SetRelativeLocation(FVector(decalLocation.X, decalLocation.Y, 1.0f));
		}
		break;
	default:
		break;
	}

	// Decal 배치 (Decal 위치가 타일맵 위에 있는 경우 + ETC 확인)
	bool canDisplayDecal = 
		decalMI != nullptr &&
		currentDecalComp != nullptr &&
		(currentDecalComp->GetRelativeLocation().X != decalLocation.X || currentDecalComp->GetRelativeLocation().Y != decalLocation.Y);
	if (canDisplayDecal) {
		currentDecalComp->SetRelativeLocation(decalLocation);
	}


}

void ATileMap::HideDecal(EDecalType DecalType)
{
	switch (DecalType) {
	case EDecalType::Normal:
		if(CursorTileDecal && CursorTileDecal != nullptr) CursorTileDecal->SetVisibility(false);
		break;
	case EDecalType::Build:		
		break;
	case EDecalType::NotBuild:
		break;
	case EDecalType::EffectArea:
		break;
	case EDecalType::Drag:
		if (DragTileDecal && DragTileDecal != nullptr) DragTileDecal->SetVisibility(false); 
		if(DragWidget && DragWidget != nullptr) DragWidget->SetVisibility(false);
		break;
	default:
		break;
	}
}

UMaterialInterface* ATileMap::GetTargetDecalMI(UMaterialInterface* MI)
{
	if (MI == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("!!!- Null : UMaterialInterface from 'GetDecalMI()' function. -!!!"));
		return nullptr;
	}
	else {
		return MI;
	}	
}

UDecalComponent* ATileMap::GetTargetDecalComponent(UDecalComponent* DecalComp, UMaterialInterface* MI, FVector Size)
{
	if (DecalComp == nullptr) {
		if (MI != nullptr && DecalComp == nullptr) {
			DecalComp = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), MI, Size, SM->GetRelativeLocation(), FRotator().ZeroRotator, 0.0f);
		}
	}
	if(!DecalComp->IsVisible())DecalComp->SetVisibility(true);
	return DecalComp;	
}

