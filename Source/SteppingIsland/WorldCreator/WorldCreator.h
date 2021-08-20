// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldCreator.generated.h"

USTRUCT(BlueprintType)
struct FCanvasMaterialTransform {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector2D Position;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector2D Size;
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
