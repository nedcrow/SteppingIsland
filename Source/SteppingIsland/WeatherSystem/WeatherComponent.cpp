// Fill out your copyright notice in the Description page of Project Settings.


#include "WeatherComponent.h"

// Sets default values for this component's properties
UWeatherComponent::UWeatherComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UWeatherComponent::BeginPlay()
{
	Super::BeginPlay();

	// SetSeason
	if (!SeasonTable) {
		UE_LOG(LogTemp, Warning, TEXT("Warning: Null SeasonDataTable in WeatherComponent"));
	}
	else {
		SeasonArr.Empty();
		for (int i = 1; i <= SeasonTable->GetRowNames().Num(); i++) {
			FSeasonStruct* ItemData = SeasonTable->FindRow<FSeasonStruct>(FName(*(FString::FromInt(i))), TEXT(""));
			if (ItemData) {
				SeasonArr.Add(*ItemData);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Warning: Not found Item in SeasonTable"));
				break;
			}
		}
	}

	// SetYearTime
	if (SeasonArr.Num() == 0) {
		UE_LOG(LogTemp, Warning, TEXT("Error : SeasonArr(TArray<FSeason>) has not items."));
	}
	else {
		for (auto seasoon : SeasonArr) {
			OneYearDays += seasoon.SeasonTime_Day;
		}
	}
}


// Called every frame
void UWeatherComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Day
	if (CurrentSecondOfDay < OneDaySeconds) {
		CurrentSecondOfDay += GetWorld()->GetDeltaSeconds();
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Y _ S _ D : %d _ %s _ %d"), CurrentYear, *SeasonArr[CurrentSeasonIndex].Name.ToString(), CurrentDayOfYear);
		CurrentSecondOfDay = 0;
		CurrentDayOfYear += 1;;
		CurrentDay.Temperature_Celsius = FMath::Floor(FMath::RandRange(-3.f, 5.f) * 10) * 0.1f;
		CurrentDay.Humidity_Percentage = FMath::Floor(FMath::RandRange(0.f, 2.f) * 10) * 0.1f;
		CurrentDay.WindSpeed_Knot = FMath::Floor(FMath::RandRange(0.f, 2.f) * 10) * 0.1f;
	}

	// Season
	int dayLimit = 0;
	for (int i = 0; i < CurrentSeasonIndex + 1; i++) {
		dayLimit += SeasonArr[i].SeasonTime_Day;
	}
	if (CurrentDayOfYear >= dayLimit + 1) {
		CurrentSeasonIndex += 1;
	}
	if (CurrentSeasonIndex >= SeasonArr.Num()) {
		CurrentSeasonIndex = 0;
	}

	//// Weather
	CurrentDay.Temperature_Celsius += SeasonArr[CurrentSeasonIndex].Temperature_Celsius;
	CurrentDay.Humidity_Percentage += SeasonArr[CurrentSeasonIndex].Humidity_Percentage;
	CurrentDay.WindSpeed_Knot += SeasonArr[CurrentSeasonIndex].WindSpeed_Knot;
	
	// Year
	if (CurrentDayOfYear >= OneYearDays) {
		CurrentYear += 1;
		CurrentDayOfYear = 1;
	}
	
}

