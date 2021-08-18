// Fill out your copyright notice in the Description page of Project Settings.


#include "TileSnapComponent.h"
#include "BuildingBase.h"
#include "../TileMap.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UTileSnapComponent::UTileSnapComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


#if WITH_EDITOR
void UTileSnapComponent::PostEditComponentMove(bool bFinished)
{
	Super::PostEditComponentMove(bFinished);
	SnapToTileMap(bFinished);
}
#endif

void UTileSnapComponent::SnapToTileMap(bool bFinished)
{
	ATileMap* tileMap = Cast<ATileMap>(UGameplayStatics::GetActorOfClass(GetWorld(), ATileMap::StaticClass()));
	if (tileMap && bFinished) {

		
		FSnapableLocation snapableLocation = GetNeedSnapableLocation(tileMap->GetTileArr(TargetTileMapLayer));

		// 가장 가까운 위치에 상위 액터를 배치 (위치에 상위 액터의 Box 크기 반영)
		float addX = 0.f;
		float addY = 0.f;
		float addZ = 0.f;

		// X, Y 크기 반영
		ABuildingBase* building = Cast<ABuildingBase>(GetOwner());
		bool isEvenX = ((int)building->Size.X) % 2 == 0 ? true : false;
		bool isEvenY = ((int)building->Size.Y) % 2 == 0 ? true : false;
		if (isEvenX) {
			addX = tileMap->TileUnit * 0.5f;
		}
		if (isEvenY) {
			addY = tileMap->TileUnit * 0.5f;
		}

		// Z 크기 반영
		UBoxComponent* tempBox = Cast<UBoxComponent>(GetOwner()->GetComponentByClass(UBoxComponent::StaticClass()));
		addZ = tempBox ? tempBox->GetScaledBoxExtent().Z : 0.f;

		FVector targetLocation = FVector(
			snapableLocation.TileLocation.X + addX,
			snapableLocation.TileLocation.Y + addY,
			snapableLocation.TileLocation.Z + addZ
		);

		// 맵 벗어난 경우 Filter
		if (building->Size.X > 1) {
			float halfBuildingSizeX = tileMap->TileUnit * building->Size.X * 0.5f;
			float halfBuildingSizeY = tileMap->TileUnit * building->Size.Y * 0.5f;
			bool isInTileMap =
				targetLocation.X - halfBuildingSizeX > -tileMap->GetHalfTileMapSize().X &&
				targetLocation.X + halfBuildingSizeX < tileMap->GetHalfTileMapSize().X&&
				targetLocation.Y - halfBuildingSizeY > -tileMap->GetHalfTileMapSize().Y &&
				targetLocation.Y + halfBuildingSizeY < tileMap->GetHalfTileMapSize().Y;
			if (!isInTileMap) {
				while (targetLocation.X - halfBuildingSizeX < -tileMap->GetHalfTileMapSize().X)
				{
					targetLocation = FVector(
						targetLocation.X + tileMap->TileUnit,
						targetLocation.Y,
						targetLocation.Z
					);
				}
				while (targetLocation.X + halfBuildingSizeX > tileMap->GetHalfTileMapSize().X)
				{
					targetLocation = FVector(
						targetLocation.X - tileMap->TileUnit,
						targetLocation.Y,
						targetLocation.Z
					);
				}
				while (targetLocation.Y - halfBuildingSizeY < -tileMap->GetHalfTileMapSize().Y)
				{
					targetLocation = FVector(
						targetLocation.X,
						targetLocation.Y + tileMap->TileUnit,
						targetLocation.Z
					);
				}
				while (targetLocation.Y + halfBuildingSizeY > tileMap->GetHalfTileMapSize().Y)
				{
					targetLocation = FVector(
						targetLocation.X,
						targetLocation.Y - tileMap->TileUnit,
						targetLocation.Z
					);
				}
			}

		}

		// 이동
		GetOwner()->SetActorRelativeLocation(targetLocation);

		// matrix 업데이트
		if (InfoOfCheckedTileMatrixProp.LayerIndex != -1) {
			tileMap->CheckTileMatrix(
				InfoOfCheckedTileMatrixProp.LayerIndex,
				false,
				InfoOfCheckedTileMatrixProp.StartX,
				InfoOfCheckedTileMatrixProp.EndX,
				InfoOfCheckedTileMatrixProp.StartY,
				InfoOfCheckedTileMatrixProp.EndY
			);
		}

		FSnapableLocation currentLocation = GetNeedSnapableLocation(tileMap->GetTileArr(TargetTileMapLayer));		
		int startIndexX = -1 * (isEvenX ? (int)building->Size.X * 0.5f - 1 : (int)building->Size.X * 0.5f - 0.5f);
		int startIndexY = -1 * (isEvenY ? (int)building->Size.Y * 0.5f - 1 : (int)building->Size.Y * 0.5f - 0.5f);
		int endIndexX = isEvenX ? (int)building->Size.X * 0.5f : (int)building->Size.X * 0.5f - 0.5f;
		int endIndexY = isEvenY ? (int)building->Size.Y * 0.5f : (int)building->Size.Y * 0.5f - 0.5f;

		tileMap->CheckTileMatrix(
			TargetTileMapLayer,
			true,
			currentLocation.IndexX - startIndexX,
			currentLocation.IndexX + endIndexX,
			currentLocation.IndexY - startIndexY,
			currentLocation.IndexY - endIndexY
		);
		
		InfoOfCheckedTileMatrixProp.LayerIndex = TargetTileMapLayer;
		InfoOfCheckedTileMatrixProp.StartX = currentLocation.IndexX - startIndexX;
		InfoOfCheckedTileMatrixProp.EndX = currentLocation.IndexX + endIndexX;
		InfoOfCheckedTileMatrixProp.StartY = currentLocation.IndexY - startIndexY;
		InfoOfCheckedTileMatrixProp.EndY = currentLocation.IndexY - endIndexY;
	}
}

FSnapableLocation UTileSnapComponent::GetNeedSnapableLocation(TArray<FTile> TileArr)
{
	// SnapableTransforms 배열에 보정된 Transform들을 담음
	FVector myLocation = FVector(
		GetOwner()->GetActorLocation().X - 0.01f,
		GetOwner()->GetActorLocation().Y - 0.01f,
		GetOwner()->GetActorLocation().Z
	);
	TArray<FSnapableLocation> snapableLocations = TArray<FSnapableLocation>();
	for (auto Tile : TileArr) {
		FSnapableLocation tempSnapableTransform;
		tempSnapableTransform.IndexX = Tile.IndexX;
		tempSnapableTransform.IndexY = Tile.IndexY;
		tempSnapableTransform.TileLocation = Tile.Location;
		tempSnapableTransform.DistanceWithThis = FVector().Dist(myLocation, Tile.Location);
		snapableLocations.Add(tempSnapableTransform);
	}

	// 거리 오름차순 정렬
	snapableLocations.Sort([](FSnapableLocation a, FSnapableLocation b) {return a.DistanceWithThis < b.DistanceWithThis; });
	return snapableLocations[0];
}
