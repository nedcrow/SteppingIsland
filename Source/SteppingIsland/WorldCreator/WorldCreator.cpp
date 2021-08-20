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

	// 분할 포인트 설정
	int tileX = 4;
	int tileY = 4;
	TArray<FVector2D> pointArr;
	pointArr.Init(FVector2D::ZeroVector, tileX * tileY);

	// 사이드 offset
	for (int i = 0; i < tileX; i++) {
		pointArr[i] = FVector2D(0.f, FMath::RandRange(.1f, .8f)); // top
		pointArr[tileX * tileY - tileX -1 + i] = FVector2D(0.f, FMath::RandRange(.1f, .8f)); // bottom
	}
	for (int i = 0; i < tileY; i++) {
		pointArr[i * tileX] = FVector2D(FMath::RandRange(.1f, .8f), 0.f); // left
		pointArr[i * tileX + tileX - 1] = FVector2D(FMath::RandRange(.1f, .8f), 0.f); // right
	}

	// 모서리
	pointArr[0] = FVector2D(FMath::RandRange(.1f, .8f), FMath::RandRange(.1f, .8f)); // top left
	pointArr[tileX - 1] = FVector2D(FMath::RandRange(.1f, .8f), FMath::RandRange(.1f, .8f)); // top right
	pointArr[tileX * tileY - tileX] = FVector2D(FMath::RandRange(.1f, .8f), FMath::RandRange(.1f, .8f)); // bottom left
	pointArr[tileX * tileY - 1] = FVector2D(FMath::RandRange(.1f, .8f), FMath::RandRange(.1f, .8f)); // bottom right


	// filling the pixels with dummy data (4 boxes: red, green, blue and white)	

	//for (int y = 0; y < height; y++)
	//{
	//	for (int x = 0; x < width; x++)
	//	{
	//		if (x < width / 2) {
	//			if (y < height / 2) {
	//				pixels[y * 4 * width + x * 4 + 0] = 255; // R
	//				pixels[y * 4 * width + x * 4 + 1] = 0;   // G
	//				pixels[y * 4 * width + x * 4 + 2] = 0;   // B
	//				pixels[y * 4 * width + x * 4 + 3] = 255; // A
	//			}
	//			else {
	//				pixels[y * 4 * width + x * 4 + 0] = 0;   // R
	//				pixels[y * 4 * width + x * 4 + 1] = 255; // G
	//				pixels[y * 4 * width + x * 4 + 2] = 0;   // B
	//				pixels[y * 4 * width + x * 4 + 3] = 255; // A
	//			}
	//		}
	//		else {
	//			if (y < height / 2) {
	//				pixels[y * 4 * width + x * 4 + 0] = 0;   // R
	//				pixels[y * 4 * width + x * 4 + 1] = 0;   // G
	//				pixels[y * 4 * width + x * 4 + 2] = 255; // B
	//				pixels[y * 4 * width + x * 4 + 3] = 255; // A
	//			}
	//			else {
	//				pixels[y * 4 * width + x * 4 + 0] = 255; // R
	//				pixels[y * 4 * width + x * 4 + 1] = 255; // G
	//				pixels[y * 4 * width + x * 4 + 2] = 255; // B
	//				pixels[y * 4 * width + x * 4 + 3] = 255; // A
	//			}
	//		}
	//	}
	//}

	
	for (int i = 0; i < width * height; i++) {
		pixels[i * 4 + 3] = 0;		
	}
	int unitSizeY = FMath::Floor(height / tileY);
	int unitSizeX = FMath::Floor(width / tileX);

	for (int pointY = 0; pointY < tileY; pointY++) {
		for (int pointX = 0; pointX < tileX; pointX++) {
			int pointIndex = tileX * pointY + pointX;
			int startY = unitSizeY * pointY;
			int endLoopY = unitSizeY * (1 + pointY);
			int offsetY = FMath::Floor(pointArr[pointIndex].Y * unitSizeY);
			UE_LOG(LogTemp, Warning, TEXT("offset: %d"), offsetY);
			if (pointY == 0) {
				startY += offsetY;
			}
			else {
				endLoopY -= offsetY;
			}

			if (pointY == 0 && pointX == 0) { UE_LOG(LogTemp, Warning, TEXT("y: %d"), startY); }
			for (int y = startY; y < endLoopY; y++) {
				int startX = unitSizeX * pointX;
				int endLoopX = unitSizeX * (1 + pointX);
				int offsetX = FMath::Floor(pointArr[pointIndex].X * unitSizeY);
				if (pointX == 0) {
					startX += offsetX;
				}
				else {
					endLoopX -= offsetX;
				}

				for (int x = startX; x < endLoopX; x++) {
					pixels[y * 4 * width + x * 4 + 0] = (tileX * pointY + pointX) * 10;
					pixels[y * 4 * width + x * 4 + 1] = (tileX * pointY + pointX) * 10;
					pixels[y * 4 * width + x * 4 + 2] = 255;
					pixels[y * 4 * width + x * 4 + 3] = 255;
				}
			}
		}
	}

	/*for (int y = 100; y < 200; y++) {
		for (int x = 0; x < 512; x++) {
			pixels[y * 4 * 1024 + x * 4 + 0] = 0;
			pixels[y * 4 * 1024 + x * 4 + 1] = 255;
			pixels[y * 4 * 1024 + x * 4 + 2] = 255;
			pixels[y * 4 * 1024 + x * 4 + 3] = 255;
		}
	}*/
	


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

