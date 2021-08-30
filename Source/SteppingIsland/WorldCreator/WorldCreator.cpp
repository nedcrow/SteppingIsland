// Fill out your copyright notice in the Description page of Project Settings.
#include "WorldCreator.h"
#include "Engine/World.h"
#include "Engine/Canvas.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Landscape.h"
#include "LandscapeComponent.h"

// Sets default values
AWorldCreator::AWorldCreator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWorldCreator::BeginPlay()
{
	Super::BeginPlay();
	
	//if (!TargetLandscape) {
	//	TargetLandscape = Cast<ALandscape>(UGameplayStatics::GetActorOfClass(GetWorld(), ALandscape::StaticClass()));
	//}

	//if (TargetLandscape) {
	//	// Tracing으로 위치 찾기
	//	FHitResult outHit;
	//	FVector traceStart = GetOwner()->GetActorLocation();
	//	FVector traceEnd = traceStart + (FVector::UpVector * -999.f);
	//	FCollisionQueryParams collisionParam;
	//	collisionParam.AddIgnoredActor(GetOwner());
	//	GetWorld()->LineTraceSingleByChannel(outHit, traceStart, traceEnd, ECollisionChannel::ECC_Visibility, collisionParam);

	//	// RenderTarget 위에 마스킹
	//	if (outHit.GetActor()) {
	//		UCanvas* canvas;
	//		FVector origin;
	//		FVector boxExtent;
	//		TargetLandscape->GetActorBounds(false, origin, boxExtent);

	//		FVector2D HitScreenPosition = FVector2D(
	//			abs(TargetLandscape->GetActorLocation().X - outHit.Location.X) / (boxExtent.X * 2),
	//			abs(TargetLandscape->GetActorLocation().Y - outHit.Location.Y) / (boxExtent.Y * 2)
	//		);
	//		FVector2D screenSize = FVector2D(RenderTarget->SizeX, RenderTarget->SizeY);
	//		FDrawToRenderTargetContext targetContext;

	//		UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), RenderTarget, canvas, screenSize, targetContext);
	//		FCanvasMaterialTransform CanvasMaterialTransform = GetCanvasMaterialTransform(HitScreenPosition, screenSize);
	//		canvas->K2_DrawMaterial(HeightBrushMI, CanvasMaterialTransform.Position, CanvasMaterialTransform.Size, FVector2D(0, 0));

	//		UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), targetContext);
	//	}
	//}

	CreateTexture();
}

// Called every frame
void AWorldCreator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UTexture2D* AWorldCreator::CreateTexture() {
	// Texture Information
	FString FileName = FString("MyTexture");
	int width = 1024;
	int height = 1024;
	uint8* pixels = (uint8*)malloc(height * width * 4); // x4 because it's RGBA. 4 integers, one for Red, one for Green, one for Blue, one for Alpha

	// 타일정보 설정
	FVector2D unitSize = FVector2D(FMath::Floor(width / TileCountX), FMath::Floor(height / TileCountY));
	//int unitSizeY = FMath::Floor(height / TileCountY);
	//int unitSizeX = FMath::Floor(width / TileCountX);

	TArray<FTextureTile> tileArr;
	FTextureTile TempTile;
	tileArr.Init(TempTile, TileCountX * TileCountY);

	for (int i = 0; i < TileCountY; i++) {
		int startIndex = FMath::RandRange(0, (int)FMath::Floor(TileCountX * 0.5f));
		int length = FMath::RandRange(1, TileCountX - startIndex);
	}

	// 사이드 offset
	for (int i = 0; i < TileCountX; i++) {
		tileArr[i].TileLength = FTextureTile::SetTileLength(unitSize, ETargetAxis::Y); // top
		tileArr[TileCountX * TileCountY - TileCountX -1 + i].TileLength = FTextureTile::SetTileLength(unitSize, ETargetAxis::Y); // bottom
	}
	for (int i = 0; i < TileCountY; i++) {
		tileArr[i * TileCountX].TileLength = FTextureTile::SetTileLength(unitSize, ETargetAxis::X);// left
		tileArr[i * TileCountX + TileCountX - 1].TileLength = FTextureTile::SetTileLength(unitSize, ETargetAxis::X); // right
	}

	// 모서리
	tileArr[0].TileLength = FTextureTile::SetTileLength(unitSize, ETargetAxis::XY); // top left
	tileArr[TileCountX - 1].TileLength = FTextureTile::SetTileLength(unitSize, ETargetAxis::XY); // top right
	tileArr[TileCountX * TileCountY - TileCountX].TileLength = FTextureTile::SetTileLength(unitSize, ETargetAxis::XY); // bottom left
	tileArr[TileCountX * TileCountY - 1].TileLength = FTextureTile::SetTileLength(unitSize, ETargetAxis::XY); // bottom right


	// filling the pixels with dummy data (4 boxes: red, green, blue and white)	
	
	for (int i = 0; i < width * height; i++) {
		pixels[i * 4 + 3] = 0;		
	}
	

	// 1차 그림
	// 한 타일 씩 픽셀을 그리고 타일별 모서리 위치를 저장
	for (int tileY = 0; tileY < TileCountY; tileY++) {
		for (int tileX = 0; tileX < TileCountX; tileX++) {
			// 박스 그리기
			int tileIndex = TileCountX * tileY + tileX;
			int startY = unitSize.Y * tileY;
			int endLoopY = unitSize.Y * (1 + tileY);
			if (tileY == 0) {
				startY = endLoopY - tileArr[tileIndex].TileLength.Y;
			}
			else if (tileY == TileCountY - 1) {
				endLoopY = startY + tileArr[tileIndex].TileLength.Y;
			}

			for (int y = startY; y < endLoopY; y++) {
				int startX = unitSize.X * tileX;
				int endLoopX = unitSize.X * (1 + tileX);
				if (tileX == 0) {
					startX = endLoopX - tileArr[tileIndex].TileLength.X;
				}
				else if (tileX == TileCountX - 1) {
					endLoopX = startX + tileArr[tileIndex].TileLength.X;
				}

				for (int x = startX; x < endLoopX; x++) {
					pixels[y * 4 * width + x * 4 + 0] = (TileCountX * tileY + tileX) * 10;
					pixels[y * 4 * width + x * 4 + 1] = (TileCountX * tileY + tileX) * 10;
					pixels[y * 4 * width + x * 4 + 2] = 255;
					pixels[y * 4 * width + x * 4 + 3] = 255;
				}

				if (y == startY) {
					tileArr[tileIndex].LTop = startY * 4 * width + startX * 4;
					tileArr[tileIndex].RTop = startY * 4 * width + (endLoopX - 1) * 4;
					tileArr[tileIndex].LBottom = (endLoopY - 1) * 4 * width + startX * 4;
					tileArr[tileIndex].RBottom = (endLoopY - 1) * 4 * width + (endLoopX - 1) * 4;
				}
			}

			// 모서리 라운딩
			// Left에서 시계방향으로 비교 포지션 설정
			TArray<FVector2D> comparisonPoints = {
				FVector2D(-1.f, 0.f),
				FVector2D(-1.f, -1.f),
				FVector2D(0.f, -1.f),
				FVector2D(1.f, -1.f),
				FVector2D(1.f, 0.f),
				FVector2D(1.f, 1.f),
				FVector2D(0.f, 1.f),
				FVector2D(-1.f, 1.f)
			};

			// 각 코너에서 비교포지션들 중 빈칸 확인
			TArray<int> currentCornerIndexArr = { tileArr[tileIndex].LTop, tileArr[tileIndex].RTop, tileArr[tileIndex].RBottom, tileArr[tileIndex].LBottom };
			for (auto cornerIndex : currentCornerIndexArr) {
				TArray<ECorner> cornerTypeArr = { ECorner::LeftTop, ECorner::RightTop, ECorner::RightBottom, ECorner::LeftBottom };
				ECorner currentCorner = ECorner::None;
				for (int i = 0; i < 4; i++) {
					int emptyPixelCount = 0;
					for (int j = 0; j < 3; j++) {
						int index = cornerIndex + (comparisonPoints[j].Y * 4 * width) + (comparisonPoints[j].X * 4);
						if (pixels[cornerIndex + 3] == 0) UE_LOG(LogTemp, Warning, TEXT("(Error)"));
						if (pixels[index + 3] == 0) emptyPixelCount++;
					}

					if (emptyPixelCount == 3) {
						if (currentCorner == ECorner::None) {
							currentCorner = cornerTypeArr[i];
						}
						else {
							currentCorner = ECorner::None;
							break;
						}
					}

					TArray<FVector2D> temp;
					temp.Add(comparisonPoints[0]);
					temp.Add(comparisonPoints[1]);
					comparisonPoints.RemoveAt(0);
					comparisonPoints.RemoveAt(0);
					comparisonPoints.Add(temp[0]);
					comparisonPoints.Add(temp[1]);
				}


				const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECorner"), true);
				if (!enumPtr)
				{
					UE_LOG(LogTemp, Warning, TEXT("(invald)"));
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("(%s)"), *enumPtr->GetNameStringByIndex((int32)currentCorner));
				}

				int tileLengthX = tileArr[tileIndex].TileLength.X;
				int tileLengthY = tileArr[tileIndex].TileLength.Y;
				int thickX;
				int thickY;
				TArray<int> xArr;
				TArray<int> yArr;
				FVector2D circlePivot;

				switch (currentCorner)
				{
				case ECorner::LeftTop:
					if (tileX != 0 && tileY == 0) {
						int preTileLengthY = tileArr[TileCountX * tileY + tileX - 1].TileLength.Y;						
						tileLengthY = tileLengthY <= preTileLengthY ? tileLengthY : tileLengthY - preTileLengthY;
					}
					if (tileY != 0 && tileX == 0) {
						int preTileLengthX = tileArr[TileCountX * (tileY - 1) + tileX].TileLength.X;
						tileLengthX = tileLengthX - preTileLengthX;
					}
					thickX = CornerThickness < tileLengthX * 0.5f ? CornerThickness : FMath::Floor(tileLengthX * 0.5f);
					thickY = CornerThickness < tileLengthY * 0.5f ? CornerThickness : FMath::Floor(tileLengthY * 0.5f);
					circlePivot = FVector2D(tileArr[tileIndex].LTop + thickX * 4, tileArr[tileIndex].LTop + thickY * height * 4 + thickX * 4);

					if (tileY == 0 || tileX == 0) {
						for (int i = 0; i < thickY; i++) {		
							float thickXf = thickX*1.f;
							float x = thickY * thickY - i * i;
							float ratio = FMath::Floor(thickXf / thickY * 1000.f) * 0.001f;
							xArr.Add((FMath::Floor(ratio * FMath::Sqrt(x))));
						}
					}

					for (int y = 0; y < xArr.Num(); y++) {
						int reverseIndex = xArr.Num() - y - 1;
						int loopCount = thickX - xArr[reverseIndex];
						for (int x = 0; x < loopCount; x++) {
							pixels[tileArr[tileIndex].LTop + (y * width * 4) + (x * 4) + 3] = 0;
						}
					}
					break;
				case ECorner::RightTop:	
					if (tileX != TileCountX - 1 && tileY == 0) {
						int nextTileLengthY = tileArr[TileCountX * tileY + tileX + 1].TileLength.Y;
						tileLengthY = tileLengthY - nextTileLengthY;
					}
					if (tileY != 0 && tileX == TileCountX-1) {
						int preTileLengthX = tileArr[TileCountX * (tileY - 1) + tileX].TileLength.X;
						tileLengthX = tileLengthX - preTileLengthX;
					}
					thickX = CornerThickness < tileLengthX * 0.5f ? CornerThickness : FMath::Floor(tileLengthX * 0.5f);
					thickY = CornerThickness < tileLengthY * 0.5f ? CornerThickness : FMath::Floor(tileLengthY * 0.5f);
					circlePivot = FVector2D(tileArr[tileIndex].RTop - thickX * 4, tileArr[tileIndex].RTop + thickY * height * 4 + thickX * 4);

					if (tileY == 0 || tileX == TileCountX - 1) {
						for (int i = 0; i < thickY; i++) {
							float thickXf = thickX * 1.f;
							float x = thickY * thickY - i * i;
							float ratio = FMath::Floor(thickXf / thickY * 1000.f) * 0.001f;
							xArr.Add((FMath::Floor(ratio * FMath::Sqrt(x))));
						}
					}

					for (int y = 0; y < xArr.Num(); y++) {
						int reverseIndex = xArr.Num() - y - 1;
						int loopCount = thickX - xArr[reverseIndex];
						for (int x = loopCount; x >= 0; x--) {
							pixels[tileArr[tileIndex].RTop + (y * width * 4) + (-x * 4) + 3] = 0;
						}
					}
					break;
				case ECorner::RightBottom:
					if (tileX != TileCountX - 1 && tileY == TileCountY - 1) {
						int nextTileLengthY = tileArr[TileCountX * tileY + tileX + 1].TileLength.Y;
						tileLengthY = tileLengthY - nextTileLengthY;
					}
					if (tileY != TileCountY - 1 && tileX == TileCountX - 1) {
						int nextTileLengthX = tileArr[TileCountX * (tileY + 1) + tileX].TileLength.X;
						tileLengthX = tileLengthX - nextTileLengthX;
					}
					thickX = CornerThickness < tileLengthX * 0.5f ? CornerThickness : FMath::Floor(tileLengthX * 0.5f);
					thickY = CornerThickness < tileLengthY * 0.5f ? CornerThickness : FMath::Floor(tileLengthY * 0.5f);
					circlePivot = FVector2D(tileArr[tileIndex].RBottom - thickX * 4, tileArr[tileIndex].RBottom + thickY * height * 4 - thickX * 4);

					if (tileY == TileCountY - 1 || tileX == TileCountX - 1) {
						for (int i = 0; i < thickY; i++) {
							float thickXf = thickX * 1.f;
							float x = thickY * thickY - i * i;
							float ratio = FMath::Floor(thickXf / thickY * 1000.f) * 0.001f;
							xArr.Add((FMath::Floor(ratio * FMath::Sqrt(x))));
						}
					}

					for (int y = 0; y < xArr.Num(); y++) {
						int reverseIndex = xArr.Num() - y - 1;
						int loopCount = thickX - xArr[reverseIndex];
						for (int x = loopCount; x >= 0; x--) {
							pixels[tileArr[tileIndex].RBottom - (y * width * 4) + (-x * 4) + 3] = 0;
						}
					}
					break;
				case ECorner::LeftBottom:
					if (tileX != 0 && tileY == TileCountY - 1) {
						int preTileLengthY = tileArr[TileCountX * tileY + tileX - 1].TileLength.Y;
						tileLengthY = tileLengthY - preTileLengthY;
					}
					if (tileY != TileCountY - 1 && tileX == 0) {
						int nextTileLengthX = tileArr[TileCountX * (tileY + 1) + tileX].TileLength.X;
						tileLengthX = tileLengthX - nextTileLengthX;
					}
					thickX = CornerThickness < tileLengthX * 0.5f ? CornerThickness : FMath::Floor(tileLengthX * 0.5f);
					thickY = CornerThickness < tileLengthY * 0.5f ? CornerThickness : FMath::Floor(tileLengthY * 0.5f);
					circlePivot = FVector2D(tileArr[tileIndex].LBottom + thickX * 4, tileArr[tileIndex].LBottom + thickY * height * 4 - thickX * 4);

					if (tileY == TileCountY - 1 || tileX == 0) {
						for (int i = 0; i < thickY; i++) {
							float thickXf = thickX * 1.f;
							float x = thickY * thickY - i * i;
							float ratio = FMath::Floor(thickXf / thickY * 1000.f) * 0.001f;
							xArr.Add((FMath::Floor(ratio * FMath::Sqrt(x))));
						}
					}

					for (int y = 0; y < xArr.Num(); y++) {
						int reverseIndex = xArr.Num() - y - 1;
						int loopCount = thickX - xArr[reverseIndex];
						for (int x = loopCount; x >= 0; x--) {
							pixels[tileArr[tileIndex].LBottom - (y * width * 4) + (x * 4) + 3] = 0;
						}
					}
					break;
				default:
					break;
				}
			}
		}
	}

	// Create Package
	FString pathPackage = FString("/Game/WorldCreator/Textures/");
	FString absolutePathPackage = FPaths::ProjectContentDir() + "/WorldCreator/Textures/";

	FPackageName::RegisterMountPoint(*pathPackage, *absolutePathPackage);

	UPackage* Package = CreatePackage(nullptr, *pathPackage); 

	// Create the Texture
	FName TextureName = MakeUniqueObjectName(Package, UTexture2D::StaticClass(), FName(*FileName));
	UTexture2D* Texture = NewObject<UTexture2D>(Package, TextureName, RF_Public | RF_Standalone);

	// Texture Settings
	Texture->PlatformData = new FTexturePlatformData();
	Texture->PlatformData->SizeX = width;
	Texture->PlatformData->SizeY = height;
	Texture->PlatformData->PixelFormat = PF_R8G8B8A8;

	// Passing the pixels information to the texture	
	FTexture2DMipMap* Mip = new(Texture->PlatformData->Mips) FTexture2DMipMap();
	Mip->SizeX = width;
	Mip->SizeY = height;
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = (uint8*)Mip->BulkData.Realloc(height * width * sizeof(uint8) * 4);
	FMemory::Memcpy(TextureData, pixels, sizeof(uint8) * height * width * 4);
	Mip->BulkData.Unlock();

	// Updating Texture & mark it as unsaved
	Texture->AddToRoot();
	Texture->UpdateResource();
	Package->MarkPackageDirty();

	UE_LOG(LogTemp, Log, TEXT("Texture created: %s"), &FileName);

	free(pixels);
	pixels = NULL;

	return Texture;
}


FCanvasMaterialTransform AWorldCreator::GetCanvasMaterialTransform(FVector2D Position, FVector2D Size, float Scale)
{
	FCanvasMaterialTransform result;

	FVector2D halfPos = Position - 0.5f;
	FVector2D halfSize = Size * 0.5f;
	result.Position = (FVector2D(Size.X, Size.Y) * FVector2D(halfPos.X, halfPos.Y)) + (halfSize * (1 - Scale));
	result.Size = Size * Scale;

	return result;
}

FVector2D FTextureTile::SetTileLength(FVector2D UnitSize, ETargetAxis TargetAxis)
{
	switch (TargetAxis)
	{
	case ETargetAxis::None:
		return FVector2D(UnitSize.X, UnitSize.Y);
	case ETargetAxis::X:
		return FVector2D(FMath::Floor(FMath::RandRange(.1f, .9f)*UnitSize.X), UnitSize.Y);
	case ETargetAxis::Y:
		return FVector2D(UnitSize.X, FMath::Floor(FMath::RandRange(.1f, .9f) * UnitSize.Y));
	case ETargetAxis::XY:
		return FVector2D(
			FMath::Floor(FMath::RandRange(.1f, .9f) * UnitSize.X), 
			FMath::Floor(FMath::RandRange(.1f, .9f) * UnitSize.Y)
		);
	default:
		return FVector2D(UnitSize.X, UnitSize.Y);
	}	
}
