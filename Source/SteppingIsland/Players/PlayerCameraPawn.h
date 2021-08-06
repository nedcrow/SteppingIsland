// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerCameraPawn.generated.h"

/*
*  유저 카메라 및 커서 인터렉션 담당.
*/
UCLASS()
class STEPPINGISLAND_API APlayerCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerCameraPawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USphereComponent* Sphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USphereComponent* Body;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UFloatingPawnMovement* PawnMovementComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void TransportToStartPoint();


	// Translate
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn")
	float TurnSpeed = 0.1f;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnRight(float Value);


	// CurrentTile checking
	/* 매 tick 마다 확인하며, GameState 프로퍼티 갱신 */
	void CheckCurrentTile();

	// Touch interaction
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
		bool bCanSelectTile = true;

private:
	AController* HitController;
	TArray<FHitResult> TraceCursor();
};
