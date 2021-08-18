// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingBase.generated.h"

UCLASS()
class STEPPINGISLAND_API ABuildingBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuildingBase();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UBoxComponent* Box;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UStaticMeshComponent* SM;

protected:
#if WITH_EDITOR
	virtual void PostRegisterAllComponents() override;
	virtual void BeginPlay() override;
#endif



// Building Size
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float BuildingUnit = 50.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector Size = FVector(1.f,1.f,1.f);
private:
	UFUNCTION()
	void SetBuildingScale(int TileUnit);
};
