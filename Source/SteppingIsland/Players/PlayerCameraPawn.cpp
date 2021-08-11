// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCameraPawn.h"
#include "../TileSystem/TileMap.h"
#include "../MainPC.h"

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
	if(bCanRotateAround) RotateAroundCamera();
	if(bCanDragTiles) DragTiles();
	CheckCurrentTile();
}

// Called to bind functionality to input
void APlayerCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCameraPawn::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APlayerCameraPawn::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("TurnRight"), this, &APlayerCameraPawn::TurnRight);
	PlayerInputComponent->BindAction(TEXT("RotateAround"), IE_Pressed, this, &APlayerCameraPawn::CallRotateAround);
	PlayerInputComponent->BindAction(TEXT("LeftClick"), IE_Pressed, this, &APlayerCameraPawn::LeftClick_Pressed);
	PlayerInputComponent->BindAction(TEXT("LeftClick"), IE_Released, this, &APlayerCameraPawn::LeftClick_Released);
}

void APlayerCameraPawn::TransportToStartPoint()
{
	// startpoint actor�� ������ �ű�� �̵�
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

void APlayerCameraPawn::LeftClick_Pressed()
{
	SetStartCursorTransform();
	StartDragTiles();
}

void APlayerCameraPawn::LeftClick_Released()
{
	EndCameraRotating();
	EndDragTiles();
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
	FRotator currentRot = RootComponent->GetRelativeRotation();
	RootComponent->SetRelativeRotation(FRotator(currentRot.Pitch, currentRot.Yaw - Value * TurnSpeed, currentRot.Roll));
}

void APlayerCameraPawn::CallRotateAround()
{
	SetStartCursorTransform();
	bCanRotateAround = true;
}

void APlayerCameraPawn::RotateAroundCamera()
{
	AMainPC* PC = Cast<AMainPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC) {
		int32 scrX;
		int32 scrY;
		FVector2D tempMousePosition;

		PC->GetViewportSize(scrX, scrY);
		PC->GetLocalPlayer()->ViewportClient->GetMousePosition(tempMousePosition);

		float addRotationPitch = ((tempMousePosition.Y - StartMousePosition.Y) / scrY * MaxRotation);
		float addRotationYaw = ((tempMousePosition.X - StartMousePosition.X) / scrX * MaxRotation);

		if (addRotationPitch != 0 && addRotationYaw != 0) {
			RootComponent->SetRelativeRotation(FRotator(
				StartRootRotator.Pitch - addRotationPitch,
				StartRootRotator.Yaw + addRotationYaw,
				0.0f
			));
		}
	}
}

void APlayerCameraPawn::EndCameraRotating()
{
	if(bCanRotateAround) bCanRotateAround = false;
}

void APlayerCameraPawn::SetStartCursorTransform()
{
	AMainPC* PC = Cast<AMainPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC) {
		PC->GetLocalPlayer()->ViewportClient->GetMousePosition(StartMousePosition);
		StartRootRotator = RootComponent->GetRelativeTransform().Rotator();
	}
}

void APlayerCameraPawn::CheckCurrentTile() {
	FHitResult tilemap = GetTilemapHitResult();
	ATileMap* TileMap = Cast<ATileMap>(UGameplayStatics::GetActorOfClass(GetWorld(), ATileMap::StaticClass()));
	if (TileMap) TileMap->SpawnHoveredDecal(tilemap.Location, tilemap.Location);
}

void APlayerCameraPawn::StartDragTiles()
{
	FHitResult tilemap = GetTilemapHitResult();
	StartDragLocation = tilemap.Location;
	if (!bCanDragTiles) bCanDragTiles = true;
}

void APlayerCameraPawn::DragTiles()
{
	FHitResult tilemap = GetTilemapHitResult();
	ATileMap* TileMap = Cast<ATileMap>(UGameplayStatics::GetActorOfClass(GetWorld(), ATileMap::StaticClass()));
	if (TileMap) TileMap->SpawnHoveredDecal(StartDragLocation, tilemap.Location, EDecalType::Drag);
}

void APlayerCameraPawn::EndDragTiles()
{
	if(bCanDragTiles) bCanDragTiles = false;
	ATileMap* TileMap = Cast<ATileMap>(UGameplayStatics::GetActorOfClass(GetWorld(), ATileMap::StaticClass()));
	if (TileMap) TileMap->HideDecal(EDecalType::Drag);
}

FHitResult APlayerCameraPawn::GetTilemapHitResult()
{
	TArray<FHitResult> outHits = GetResultOfCursorTracing();
	for (auto hit : outHits) {
		if (hit.GetActor() && hit.GetActor()->GetComponentsByTag(UStaticMeshComponent::StaticClass(), FName("Tile")).Num() > 0) {
			return hit;
		}
	}
	return FHitResult();
}

TArray<FHitResult> APlayerCameraPawn::GetResultOfCursorTracing()
{
	TArray<FHitResult> OutHits;

	if (!bCanTraceTile) {
		return OutHits;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->IsLocalController()) {

		// tracing �غ�
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

		// ��ֹ��� ����� multi tracing
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