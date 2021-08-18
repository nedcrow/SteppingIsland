// Fill out your copyright notice in the Description page of Project Settings.


#include "TileMapEventManager.h"

#include "Kismet/GameplayStatics.h"

ATileMapEventManager* ATileMapEventManager::Instance_;

ATileMapEventManager* ATileMapEventManager::GetInstance()
{
	ATileMapEventManager* eventM;
	if (Instance_) {
		eventM = Instance_;
	}
	else {
		UWorld* world = GEngine->GameViewport->GetWorld();
		eventM = Cast<ATileMapEventManager>(UGameplayStatics::GetActorOfClass(world, ATileMapEventManager::StaticClass()));
		if (!eventM) {
			eventM = world->SpawnActor<ATileMapEventManager>();
#if WITH_EDITOR
			eventM->SetFolderPath(TEXT("/Managers"));
#endif // WITH_EDITOR
		}
	}	
	return eventM;
}

// Sets default values
ATileMapEventManager::ATileMapEventManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATileMapEventManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATileMapEventManager::PostChangedTileUnit(int Value)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (PC->IsLocalController()) {
		if (F_ChangedTileUnitEvent.IsBound() == true) {
			F_ChangedTileUnitEvent.Broadcast(Value);
		}
	}
}

