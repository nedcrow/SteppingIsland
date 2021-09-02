// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldCreator.generated.h"

UENUM(BlueprintType)
enum class ETargetAxis : uint8
{
	None = 0		UMETA(DisplayName = "None"),
	X = 1			UMETA(DisplayName = "X"),
	Y = 2			UMETA(DisplayName = "Y"),
	XY = 3			UMETA(DisplayName = "XY"),
};

UENUM(BlueprintType)
enum class ECornerType_Outer : uint8
{
	None = 0				UMETA(DisplayName = "None"),
	LeftTop = 1				UMETA(DisplayName = "LeftTop"),
	RightTop = 2			UMETA(DisplayName = "RightTop"),
	LeftBottom = 3			UMETA(DisplayName = "LeftBottom"),
	RightBottom = 4			UMETA(DisplayName = "RightBottom"),
};

UENUM(BlueprintType)
enum class ECornerType_Inner : uint8
{
	None = 0				UMETA(DisplayName = "None"),
	LeftBottomL = 1			UMETA(DisplayName = "LeftBottomL"),
	RightTopT = 2			UMETA(DisplayName = "RightTopT"),
	LeftTopT = 3			UMETA(DisplayName = "LeftTopT"),
	RightBottomR = 4		UMETA(DisplayName = "RightBottomR"),
	RightTopR = 5			UMETA(DisplayName = "RightTopR"),
	LeftBottomB = 6			UMETA(DisplayName = "LeftBottomB"),
	RightBottomB = 7		UMETA(DisplayName = "RightBottomB"),
	LeftTopL = 8			UMETA(DisplayName = "LeftTopL"),
};

UENUM(BlueprintType)
enum class EBrushVector : uint8
{
	IncX = 0				UMETA(DisplayName = "IncX"),
	DecX = 1				UMETA(DisplayName = "DecX"),
	IncY = 2				UMETA(DisplayName = "IncX"),
	DecY = 3				UMETA(DisplayName = "DecY"),	
};

USTRUCT(BlueprintType)
struct FCanvasMaterialTransform {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector2D Position;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector2D Size;
};

USTRUCT(BlueprintType)
struct FTextureTile {
	GENERATED_BODY()
public:
	/* 해당 프로퍼티 요소는 int여야함. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector2D TileLength;	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int LTop;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int RTop;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int LBottom;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int RBottom;	

	static FVector2D SetTileLength(FVector2D UnitSize, ETargetAxis TargetAxis);
};

UCLASS()
class STEPPINGISLAND_API AWorldCreator : public AActor
{
	GENERATED_BODY()
	
public:	
	AWorldCreator();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data|Base", Meta = (ClampMin = "1"))
	int LayerMaxCount = 5;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data|Base", Meta = (ClampMin = "1", ClampMax = 512))
	int TileCountX = 4;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data|Base", Meta = (ClampMin = "1", ClampMax = 512))
	int TileCountY = 4;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data|Base")
	int CornerThickness = 64;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data|Texture", Meta = (ClampMin = "16"))
	int Width = 1024;	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data|Texture", Meta = (ClampMin = "16"))
	int Height = 1024;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data|Option")
	uint8 bUseRounding_Outer : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data|Option")
	uint8 bUseRounding_Inner : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	class UTextureRenderTarget2D* RenderTarget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInterface* HeightBrushMI;

	/* reference http://www.danielmayor.com/ue4-create-texture-editor-plugin */
	UFUNCTION()
	UTexture2D* CreateTexture();

	UFUNCTION()
	FCanvasMaterialTransform GetCanvasMaterialTransform(FVector2D Position, FVector2D Size, float Scale=1.f);

// Texture Information
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TileInformation")
	TArray<FTextureTile> TileArr;

	UFUNCTION()	
	TArray<int> GetRoundIndexesOfInner(int ThickX, int ThickY, int TileIndex, EBrushVector XVector, EBrushVector YVector, int AddX=0, int AddY=0);

	UFUNCTION()
	TArray<int> GetRoundIndexesOfOuter(int TileLengthX, int TileLengthY, int TileIndex, EBrushVector XVector, EBrushVector YVector);
};
