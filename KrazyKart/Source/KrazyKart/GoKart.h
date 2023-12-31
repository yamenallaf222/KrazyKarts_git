// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKartMovementComponent.h"
#include "GoKartMovementReplicator.h"
#include "GoKart.generated.h"




UCLASS()
class KRAZYKART_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UGoKartMovementComponent* GetGoKartMovementComponent() { return MovementComponent; }
	

private:


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "True"))
	UGoKartMovementComponent* MovementComponent;	
	
	UPROPERTY(VisibleAnywhere , BlueprintReadOnly , meta = (AllowPrivateAccess = "True"))
	UGoKartMovementReplicator* MovementReplicator;







	void Local_MoveForward(float Value);

	void Local_MoveRight(float Value);


	FString GetTextRole(ENetRole);




};
