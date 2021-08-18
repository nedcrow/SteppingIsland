// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "WeatherComponent.generated.h"

USTRUCT(Atomic, BlueprintType)
struct STEPPINGISLAND_API FSeasonStruct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Name;
	/* 0미만이면 해당 계절은 사용하지 않음 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_WeatherOption", Meta = (ClampMin = "1"))
		int SeasonTime_Day = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Temperature_Celsius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Humidity_Percentage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float WindSpeed_Knot; // 0.5 m/s
};

USTRUCT(Atomic, BlueprintType)
struct FDay
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Temperature_Celsius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Humidity_Percentage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float WindSpeed_Knot;
};
//
//USTRUCT(Atomic, BlueprintType)
//struct FSeason: public FDay
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FName Name;
//	/* 0미만이면 해당 계절은 사용하지 않음 */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data_WeatherOption", Meta = (ClampMin = "1"))
//		int SeasonTime_Day = 1;
//};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STEPPINGISLAND_API UWeatherComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeatherComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

// CurrentProps
public:

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "CurrentProps")
		int OneYearDays = 0;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "CurrentProps")
		int CurrentYear = 1;


	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "CurrentProps")
		int CurrentSeasonIndex = 0;


	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "CurrentProps")
		FDay CurrentDay;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "CurrentProps")
		int CurrentDayOfYear = 1;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "CurrentProps")
		float CurrentSecondOfDay = 0.f;

// WeatherOption
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		UDataTable* SeasonTable;

	/* SeasonDataTable 사용 시 데이터 테이블로 덮어쓰기된다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		TArray<FSeasonStruct> SeasonArr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", Meta = (ClampMin="1"))
		int OneDaySeconds = 1;
};
