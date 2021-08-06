// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCameraPawn.h"
#include "../TileSystem/TileMap.h"

#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
APlayerCameraPawn::APlayerCameraPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<USphereComponent>(TEXT("Body"));
	RootComponent = Body;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Body);
	SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	SpringArm->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	PawnMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("PawnMovement"));

	bUseControllerRotationYaw = true;
}

// Called when the game starts or when spawned
void APlayerCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	TransportToStartPoint();
}

// Called every frame
void APlayerCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckCurrentTile();
}

// Called to bind functionality to input
void APlayerCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCameraPawn::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APlayerCameraPawn::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("TurnRight"), this, &APlayerCameraPawn::TurnRight);
}

void APlayerCameraPawn::TransportToStartPoint()
{
	// startpoint actor가 있으면 거기로 이동
	/*TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AStartPointTile::StaticClass(), FName("StartPoint"), OutActors);
	if (OutActors.Num() > 0) {
		RootComponent->SetRelativeLocation(FVector(.0f, .0f, 200.0f));
		for (auto actor : OutActors) {
			if (actor->ActorHasTag(TEXT("Camper"))) {
				RootComponent->SetRelativeLocation(FVector(actor->GetActorLocation().X, actor->GetActorLocation().Y, 200.0f));
				break;
			}
		}
	}
	else {*/
		RootComponent->SetRelativeLocation(FVector(.0f, .0f, 200.0f));
	//}
	if (RootComponent->GetRelativeRotation().Yaw != 0) {
		AddControllerYawInput((GetControlRotation().Yaw < 0 ? GetControlRotation().Yaw : -GetControlRotation().Yaw) * 2);
	}
}

void APlayerCameraPawn::MoveForward(float Value)
{
	if (Value == 0)
	{
		return;
	}

	FVector ControlForwardVector2D = GetControlRotation().Vector().GetSafeNormal2D();
	AddMovementInput(ControlForwardVector2D, Value);
}

void APlayerCameraPawn::MoveRight(float Value)
{
	if (Value == 0)
	{
		return;
	}

	FVector ControlRightVector2D = GetControlRotation().RotateVector(FVector::RightVector).GetSafeNormal2D();
	AddMovementInput(ControlRightVector2D, Value);
}

void APlayerCameraPawn::TurnRight(float Value)
{
	if (Value == 0)
	{
		return;
	}

	AddControllerYawInput(Value * TurnSpeed);
}

void APlayerCameraPawn::CheckCurrentTile() {
	TArray<FHitResult> outHits = TraceCursor();
	for (auto hit : outHits) {
		if (hit.GetActor() && hit.GetActor()->GetComponentsByTag(UStaticMeshComponent::StaticClass(), FName("Tile")).Num() > 0) {
			ATileMap* TileMap = Cast<ATileMap>(UGameplayStatics::GetActorOfClass(GetWorld(), ATileMap::StaticClass()));
			if (TileMap) TileMap->SpawnHoveredDecal(hit.Location);
		}
	}	
}

TArray<FHitResult> APlayerCameraPawn::TraceCursor()
{
	TArray<FHitResult> OutHits;

	if (!bCanSelectTile) {
		return OutHits;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->IsLocalController()) {

		// tracing 준비
		float MouseX;
		float MouseY;
		FVector CursorWorldPosition;
		FVector CursorWorldDirection;

		FVector CameraLocation;
		FRotator CameraRotation;

		PC->GetMousePosition(MouseX, MouseY);
		PC->DeprojectScreenPositionToWorld(MouseX, MouseY, CursorWorldPosition, CursorWorldDirection);

		PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector TraceStart = CameraLocation;
		FVector TraceEnd = TraceStart + (CursorWorldDirection * 99999.f);

		TArray<TEnumAsByte<EObjectTypeQuery>> Objects;
		Objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
		Objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
		Objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
		Objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));
		Objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel2));

		TArray<AActor*> ActorToIgnore;

		// 장애물을 대비한 multi tracing
		bool Result = UKismetSystemLibrary::LineTraceMultiForObjects(
			GetWorld(),
			TraceStart,
			TraceEnd,
			Objects,
			true,
			ActorToIgnore,
			EDrawDebugTrace::None,
			OutHits,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			100.0f
		);
		return OutHits;
	}
	return OutHits;
}