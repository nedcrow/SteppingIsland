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
enum class ECorner : uint8
{
	None = 0				UMETA(DisplayName = "None"),
	LeftTop = 1				UMETA(DisplayName = "LeftTop"),
	RightTop = 2			UMETA(DisplayName = "RightTop"),
	LeftBottom = 3			UMETA(DisplayName = "LeftBottom"),
	RightBottom = 4			UMETA(DisplayName = "RightBottom"),
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


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data", Meta = (ClampMin = "1"))
	int LayerMaxCount = 5;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data", Meta = (ClampMin = "1", ClampMax = 512))
	int TileCountX = 4;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data", Meta = (ClampMin = "1", ClampMax = 512))
	int TileCountY = 4;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	int CornerThickness = 64;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	class UTextureRenderTarget2D* RenderTarget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInterface* HeightBrushMI;

	/* reference http://www.danielmayor.com/ue4-create-texture-editor-plugin */
	UFUNCTION()
	UTexture2D* CreateTexture();

	UFUNCTION()
	FCanvasMaterialTransform GetCanvasMaterialTransform(FVector2D Position, FVector2D Size, float Scale=1.f);
};
