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
	uint8* pixels = (uint8*)malloc(Height * Width * 4); // x4 because it's RGBA. 4 integers, one for Red, one for Green, one for Blue, one for Alpha

	// 타일정보 설정
	FVector2D unitSize = FVector2D(FMath::Floor(Width / TileCountX), FMath::Floor(Height / TileCountY));
	FTextureTile TempTile;
	TileArr.Empty();
	TileArr.Init(TempTile, TileCountX * TileCountY);

	// 사이드 offset 설정
	for (int i = 0; i < TileCountX; i++) {
		TileArr[i].TileLength = FTextureTile::SetTileLength(unitSize, ETargetAxis::Y); // top
		TileArr[TileCountX * TileCountY - TileCountX -1 + i].TileLength = FTextureTile::SetTileLength(unitSize, ETargetAxis::Y); // bottom
	}
	for (int i = 0; i < TileCountY; i++) {
		TileArr[i * TileCountX].TileLength = FTextureTile::SetTileLength(unitSize, ETargetAxis::X);// left
		TileArr[i * TileCountX + TileCountX - 1].TileLength = FTextureTile::SetTileLength(unitSize, ETargetAxis::X); // right
	}

	// 모서리 offset 설정
	TileArr[0].TileLength = FTextureTile::SetTileLength(unitSize, ETargetAxis::XY); // top left
	TileArr[TileCountX - 1].TileLength = FTextureTile::SetTileLength(unitSize, ETargetAxis::XY); // top right
	TileArr[TileCountX * TileCountY - TileCountX].TileLength = FTextureTile::SetTileLength(unitSize, ETargetAxis::XY); // bottom left
	TileArr[TileCountX * TileCountY - 1].TileLength = FTextureTile::SetTileLength(unitSize, ETargetAxis::XY); // bottom right
	
	// 1차 그림(4 boxes: red, green, blue and white)		
	// 전 범위를 투명하게 만들고 한 타일 씩 픽셀을 그림
	int alphaValue = 255.f;//LayerMaxCount;
	for (int i = 0; i < Width * Height; i++) {
		pixels[i * 4 + 0] = 255;
		pixels[i * 4 + 1] = 255;
		pixels[i * 4 + 2] = 255;
		pixels[i * 4 + 3] = 0;
	}

	for (int tileY = 0; tileY < TileCountY; tileY++) {
		for (int tileX = 0; tileX < TileCountX; tileX++) {
			// 타일 크기 설정
			int tileIndex = TileCountX * tileY + tileX;
			int startY = unitSize.Y * tileY;
			int endLoopY = unitSize.Y * (1 + tileY);
			if (tileY == 0) {
				startY = endLoopY - TileArr[tileIndex].TileLength.Y;
			}
			else if (tileY == TileCountY - 1) {
				endLoopY = startY + TileArr[tileIndex].TileLength.Y;
			}

			// 박스 생성 및 박스 모서리 위치 저장
			for (int y = startY; y < endLoopY; y++) {
				int startX = unitSize.X * tileX;
				int endLoopX = unitSize.X * (1 + tileX);
				if (tileX == 0) {
					startX = endLoopX - TileArr[tileIndex].TileLength.X;
				}
				else if (tileX == TileCountX - 1) {
					endLoopX = startX + TileArr[tileIndex].TileLength.X;
				}

				for (int x = startX; x < endLoopX; x++) {
					pixels[y * 4 * Width + x * 4 + 3] = alphaValue;
				}

				if (y == startY) {
					TileArr[tileIndex].LTop = startY * 4 * Width + startX * 4;
					TileArr[tileIndex].RTop = startY * 4 * Width + (endLoopX - 1) * 4;
					TileArr[tileIndex].LBottom = (endLoopY - 1) * 4 * Width + startX * 4;
					TileArr[tileIndex].RBottom = (endLoopY - 1) * 4 * Width + (endLoopX - 1) * 4;
				}
			}
		}
	}

	// 모서리 라운딩
	for (int tileY = 0; tileY < TileCountY; tileY++) {
		for (int tileX = 0; tileX < TileCountX; tileX++) {
			int tileIndex = TileCountX * tileY + tileX;

			// Left(-1, 0)에서 시계방향으로 비교 포지션 설정
			TArray<FVector2D> comparisonPoints;
			TArray<FVector2D> comparisonPoints2; 
			comparisonPoints = comparisonPoints2 = {
				FVector2D(-1.f, 0.f),
				FVector2D(-1.f, -1.f),
				FVector2D(0.f, -1.f),
				FVector2D(1.f, -1.f),
				FVector2D(1.f, 0.f),
				FVector2D(1.f, 1.f),
				FVector2D(0.f, 1.f),
				FVector2D(-1.f, 1.f)
			};

			TArray<int> currentCornerIndexArr = { TileArr[tileIndex].LTop, TileArr[tileIndex].RTop, TileArr[tileIndex].RBottom, TileArr[tileIndex].LBottom };
			for (auto cornerIndex : currentCornerIndexArr) {
				// 각 모서리에서 비교포지션들 중 빈칸 확인해서 코너타입 확인
				int tileLengthX = TileArr[tileIndex].TileLength.X;
				int tileLengthY = TileArr[tileIndex].TileLength.Y;
				int thickX;
				int thickY;
				TArray<int> xArr;
				TArray<int> yArr;

				// 안쪽 모서리 (픽셀의 알파값 변화가 발생하니 바깥보다 안쪽 먼저 실행)
				// thick 차이 - 길이 설정 시 비교 대상에서 자신을 마이너스(outer는 반대), CornerThickness 무시하고 thickX(Y)길이의 반 사용
				// 한 블럭에 2개 이상이 가능함.
				if (bUseRounding_Inner) {
					TArray<int> emptyPixelIndexArr;
					ECornerType_Inner currentCorner_Inner = ECornerType_Inner::None;
					for (int i = 0; i < comparisonPoints2.Num(); i++) {
						int index = cornerIndex + (comparisonPoints2[i].Y * 4 * Width) + (comparisonPoints2[i].X * 4);
						if (pixels[cornerIndex + 3] == 0) UE_LOG(LogTemp, Warning, TEXT("(Error)"));
						if (pixels[index + 3] == 0) emptyPixelIndexArr.Add(i);
						UE_LOG(LogTemp, Warning, TEXT("Error: index: %d, (%f, %f), alpha: %d"), i, comparisonPoints2[i].X, comparisonPoints2[i].Y, pixels[index + 3]);
					}

					TArray<ECornerType_Inner> subCornerTypeArr = { ECornerType_Inner::LeftBottomL, ECornerType_Inner::RightTopT, ECornerType_Inner::LeftTopT, ECornerType_Inner::RightBottomR,
						ECornerType_Inner::RightTopR, ECornerType_Inner::LeftBottomB, ECornerType_Inner::RightBottomB, ECornerType_Inner::LeftTopL };
					if (emptyPixelIndexArr.Num() == 2) {
						if (emptyPixelIndexArr[0] == 0) {
							if (emptyPixelIndexArr[1] == 1) {
								currentCorner_Inner = ECornerType_Inner::LeftBottomL;
							}
							else {
								currentCorner_Inner = ECornerType_Inner::LeftTopL;
							}
						}
						else {
							currentCorner_Inner = subCornerTypeArr[emptyPixelIndexArr[0]];
						}
					}

					const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECornerType_Inner"), true);
					if (!enumPtr)
					{
						UE_LOG(LogTemp, Warning, TEXT("(invald)"));
					}
					else {
						UE_LOG(LogTemp, Warning, TEXT("currentCorner_Inner: (%s)"), *enumPtr->GetNameStringByIndex((int32)currentCorner_Inner));
					}

					// 해당 안쪽 코너 타입에 맞는 모서리 그리기.
					switch (currentCorner_Inner)
					{
					case ECornerType_Inner::LeftBottomL:
					{
						int nextTileLengthX = TileArr[(tileY + 1) * TileCountX + tileX].TileLength.X;
						tileLengthX = nextTileLengthX - tileLengthX;
						thickX = CornerThickness < tileLengthX * 0.5f ? CornerThickness : FMath::Floor(tileLengthX * 0.5f);
						thickY = tileLengthY * 0.5f;
						TArray<int>indexes = GetRoundIndexesOfInner(thickX, thickY, TileArr[tileIndex].LBottom, EBrushVector::DecX, EBrushVector::DecY, 0, 0);
						for (auto index : indexes) { pixels[index] = alphaValue; }
						break;
					}
					case ECornerType_Inner::RightTopT:
					{
						int nextTileLengthY = TileArr[tileY * TileCountX + tileX + 1].TileLength.Y;
						tileLengthY = nextTileLengthY - tileLengthY;
						thickX = tileLengthX * 0.5f;
						thickY = CornerThickness < tileLengthY * 0.5f ? CornerThickness : FMath::Floor(tileLengthY * 0.5f); 						
						TArray<int>indexes = GetRoundIndexesOfInner(thickX, thickY, TileArr[tileIndex].RTop, EBrushVector::DecX, EBrushVector::DecY, 1, 0);
						for (auto index : indexes) { pixels[index] = alphaValue; }
						break;
					}
					case ECornerType_Inner::LeftTopT:
					{
						int preTileLengthY = TileArr[tileY * TileCountX + tileX - 1].TileLength.Y;
						tileLengthY = preTileLengthY - tileLengthY;
						thickX = tileLengthX * 0.5f;
						thickY = CornerThickness < tileLengthY * 0.5f ? CornerThickness : FMath::Floor(tileLengthY * 0.5f);
						TArray<int>indexes = GetRoundIndexesOfInner(thickX, thickY, TileArr[tileIndex].LTop, EBrushVector::IncX, EBrushVector::DecY, 0, 0);
						for (auto index : indexes) { pixels[index] = alphaValue; }
						break;
					}
					case ECornerType_Inner::RightBottomR:
					{
						int nextTileLengthX = TileArr[(tileY + 1) * TileCountX + tileX].TileLength.X;
						tileLengthX = nextTileLengthX - tileLengthX;
						thickX = CornerThickness < tileLengthX * 0.5f ? CornerThickness : FMath::Floor(tileLengthX * 0.5f);
						thickY = tileLengthY * 0.5f;
						TArray<int> indexes = GetRoundIndexesOfInner(thickX, thickY, TileArr[tileIndex].RBottom, EBrushVector::IncX, EBrushVector::DecY, 0, 0);
						for (auto index : indexes) { pixels[index] = alphaValue; }
						break;
					}
					case ECornerType_Inner::RightTopR: 
					{
						int preTileLengthX = TileArr[(tileY - 1) * TileCountX + tileX].TileLength.X;
						tileLengthX = preTileLengthX - tileLengthX;
						thickX = CornerThickness < tileLengthX * 0.5f ? CornerThickness : FMath::Floor(tileLengthX * 0.5f);
						thickY = tileLengthY * 0.5f;
						TArray<int> indexes = GetRoundIndexesOfInner(thickX, thickY, TileArr[tileIndex].RTop, EBrushVector::IncX, EBrushVector::IncY, 0, 0);
						for (auto index : indexes) { pixels[index] = alphaValue; }
						break;
					}
					case ECornerType_Inner::LeftBottomB:
					{
						int preTileLengthY = TileArr[tileY * TileCountX + tileX - 1].TileLength.Y;
						tileLengthY = preTileLengthY - tileLengthY;
						thickX = tileLengthX * 0.5f;
						thickY = CornerThickness < tileLengthY * 0.5f ? CornerThickness : FMath::Floor(tileLengthY * 0.5f);
						TArray<int> indexes = GetRoundIndexesOfInner(thickX, thickY, TileArr[tileIndex].LBottom, EBrushVector::IncX, EBrushVector::IncY, 0, 0);
						for (auto index : indexes) { pixels[index] = alphaValue; }
						break;
					}
					case ECornerType_Inner::RightBottomB:
					{
						int nextTileLengthY = TileArr[tileY * TileCountX + tileX + 1].TileLength.Y;
						tileLengthY = nextTileLengthY - tileLengthY;
						thickX = tileLengthX * 0.5f;
						thickY = CornerThickness < tileLengthY * 0.5f ? CornerThickness : FMath::Floor(tileLengthY * 0.5f);
						TArray<int>indexes = GetRoundIndexesOfInner(thickX, thickY, TileArr[tileIndex].RBottom, EBrushVector::DecX, EBrushVector::IncY, 1, 0);
						for (auto index : indexes) { pixels[index] = alphaValue; }
						break;
					}
					case ECornerType_Inner::LeftTopL:
					{
						int preTileLengthX = TileArr[(tileY - 1) * TileCountX + tileX].TileLength.X;
						tileLengthX = preTileLengthX - tileLengthX;
						thickX = CornerThickness < tileLengthX * 0.5f ? CornerThickness : FMath::Floor(tileLengthX * 0.5f);
						thickY = tileLengthY * 0.5f;
						TArray<int> indexes = GetRoundIndexesOfInner(thickX, thickY, TileArr[tileIndex].LTop, EBrushVector::DecX, EBrushVector::IncY, 0, 0);
						for (auto index : indexes) { pixels[index] = alphaValue; }
						break;
					}
					default:
						break;
					}
				}

				// 바깥 모서리
				if (bUseRounding_Outer) {
					TArray<ECornerType_Outer> cornerTypeArr = { ECornerType_Outer::LeftTop, ECornerType_Outer::RightTop, ECornerType_Outer::RightBottom, ECornerType_Outer::LeftBottom };
					ECornerType_Outer currentCorner_Outer = ECornerType_Outer::None;
					for (int i = 0; i < 4; i++) {
						int emptyPixelCount_Outer = 0;
						for (int j = 0; j < 3; j++) {
							int index = cornerIndex + (comparisonPoints[j].Y * 4 * Width) + (comparisonPoints[j].X * 4);
							if (pixels[cornerIndex + 3] == 0) UE_LOG(LogTemp, Warning, TEXT("(Error)"));
							if (pixels[index + 3] == 0) emptyPixelCount_Outer++;
						}

						if (emptyPixelCount_Outer == 3) {
							if (currentCorner_Outer == ECornerType_Outer::None) {
								currentCorner_Outer = cornerTypeArr[i];
							}
							else {
								currentCorner_Outer = ECornerType_Outer::None;
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

					// 현재 코너 확인
					/*const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECornerType_Outer"), true);
					if (!enumPtr)
					{
						UE_LOG(LogTemp, Warning, TEXT("(invald)"));
					}
					else {
						UE_LOG(LogTemp, Warning, TEXT("CurrentCorner: (%s)"), *enumPtr->GetNameStringByIndex((int32)currentCorner_Outer));
					}*/

					// 해당 바깥 코너 타입에 맞는 모서리 깎기.
					switch (currentCorner_Outer)
					{
					case ECornerType_Outer::LeftTop:
					{
						if (tileX != 0 && tileY == 0) {
							int preTileLengthY = TileArr[TileCountX * tileY + tileX - 1].TileLength.Y;
							tileLengthY = tileLengthY <= preTileLengthY ? tileLengthY : tileLengthY - preTileLengthY;
						}
						if (tileY != 0 && tileX == 0) {
							int preTileLengthX = TileArr[TileCountX * (tileY - 1) + tileX].TileLength.X;
							tileLengthX = tileLengthX - preTileLengthX;
						}
						TArray<int> indexes = GetRoundIndexesOfOuter(tileLengthX, tileLengthY, TileArr[tileIndex].LTop, EBrushVector::IncX, EBrushVector::IncY);
						for (auto index : indexes) { pixels[index] = 0; }
						break; 
					}
					case ECornerType_Outer::RightTop:
					{
						if (tileX != TileCountX - 1 && tileY == 0) {
							int nextTileLengthY = TileArr[TileCountX * tileY + tileX + 1].TileLength.Y;
							tileLengthY = tileLengthY - nextTileLengthY;
						}
						if (tileY != 0 && tileX == TileCountX - 1) {
							int preTileLengthX = TileArr[TileCountX * (tileY - 1) + tileX].TileLength.X;
							tileLengthX = tileLengthX - preTileLengthX;
						}
						TArray<int> indexes = GetRoundIndexesOfOuter(tileLengthX, tileLengthY, TileArr[tileIndex].RTop, EBrushVector::DecX, EBrushVector::IncY);
						for (auto index : indexes) { pixels[index] = 0; }
						break;
					}
					case ECornerType_Outer::RightBottom:
					{
						if (tileX != TileCountX - 1 && tileY == TileCountY - 1) {
							int nextTileLengthY = TileArr[TileCountX * tileY + tileX + 1].TileLength.Y;
							tileLengthY = tileLengthY - nextTileLengthY;
						}
						if (tileY != TileCountY - 1 && tileX == TileCountX - 1) {
							int nextTileLengthX = TileArr[TileCountX * (tileY + 1) + tileX].TileLength.X;
							tileLengthX = tileLengthX - nextTileLengthX;
						}
						TArray<int> indexes = GetRoundIndexesOfOuter(tileLengthX, tileLengthY, TileArr[tileIndex].RBottom, EBrushVector::DecX, EBrushVector::DecY);
						for (auto index : indexes) { pixels[index] = 0; }
						break;
					}
					case ECornerType_Outer::LeftBottom:
					{
						if (tileX != 0 && tileY == TileCountY - 1) {
							int preTileLengthY = TileArr[TileCountX * tileY + tileX - 1].TileLength.Y;
							tileLengthY = tileLengthY - preTileLengthY;
						}
						if (tileY != TileCountY - 1 && tileX == 0) {
							int nextTileLengthX = TileArr[TileCountX * (tileY + 1) + tileX].TileLength.X;
							tileLengthX = tileLengthX - nextTileLengthX;
						}
						TArray<int> indexes = GetRoundIndexesOfOuter(tileLengthX, tileLengthY, TileArr[tileIndex].LBottom, EBrushVector::IncX, EBrushVector::DecY);
						for (auto index : indexes) { pixels[index] = 0; }
						break;
					}
					default:
						break;
					}
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
	Texture->PlatformData->SizeX = Width;
	Texture->PlatformData->SizeY = Height;
	Texture->PlatformData->PixelFormat = PF_R8G8B8A8;

	// Passing the pixels information to the texture	
	FTexture2DMipMap* Mip = new(Texture->PlatformData->Mips) FTexture2DMipMap();
	Mip->SizeX = Width;
	Mip->SizeY = Height;
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = (uint8*)Mip->BulkData.Realloc(Height * Width * sizeof(uint8) * 4);
	FMemory::Memcpy(TextureData, pixels, sizeof(uint8) * Height * Width * 4);
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

TArray<int> AWorldCreator::GetRoundIndexesOfInner(int ThickX, int ThickY, int TileIndex, EBrushVector XVector, EBrushVector YVector, int AddX, int AddY)
{
	TArray<int> indexArr;
	TArray<int> xArr;
	for (int i = 0; i < ThickY; i++) {
		float thickXfloat = ThickX * 1.f;
		float x = ThickY * ThickY - i * i;
		float ratio = FMath::Floor(thickXfloat / ThickY * 1000.f) * 0.001f;
		xArr.Add((FMath::Floor(ratio * FMath::Sqrt(x))));
	}

	for (int y = 0; y < xArr.Num(); y++) {
		int reverseIndex = xArr.Num() - y - 1;
		int loopCount = ThickX - xArr[reverseIndex];
		int yVector = YVector == EBrushVector::IncY ? 1 : -1;
		for (int x = 0; x < loopCount; x++) {
			int tempX = XVector == EBrushVector::IncX ? x : -(loopCount - x);
			indexArr.Add(TileIndex + (yVector * (y - AddY) * Width * 4) + ((tempX + AddX) * 4) + 3);
		}
	}

	return indexArr;
}

TArray<int> AWorldCreator::GetRoundIndexesOfOuter(int TileLengthX, int TileLengthY, int TileIndex, EBrushVector XVector, EBrushVector YVector)
{
	int thickX = CornerThickness < TileLengthX * 0.5f ? CornerThickness : FMath::Floor(TileLengthX * 0.5f);
	int thickY = CornerThickness < TileLengthY * 0.5f ? CornerThickness : FMath::Floor(TileLengthY * 0.5f);
	TArray<int> indexArr;
	TArray<int> xArr;	

	for (int i = 0; i < thickY; i++) {
		float thickXf = thickX * 1.f;
		float x = thickY * thickY - i * i;
		float ratio = FMath::Floor(thickXf / thickY * 1000.f) * 0.001f;
		xArr.Add((FMath::Floor(ratio * FMath::Sqrt(x))));
	}

	for (int y = 0; y < xArr.Num(); y++) {
		int reverseIndex = xArr.Num() - y - 1;
		int loopCount = thickX - xArr[reverseIndex];
		int yVector = YVector == EBrushVector::IncY ? 1 : -1;
		for (int x = 0; x < loopCount; x++) {
			int tempX = XVector == EBrushVector::IncX ? x : -(loopCount - 1 - x);
			indexArr.Add(TileIndex + (yVector * y * Width * 4) + (tempX * 4) + 3);
		}
	}
	return indexArr;
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
