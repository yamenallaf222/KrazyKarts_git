// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
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

private:
	

	//mass of the car in kg 
	UPROPERTY(EditAnywhere)
	float Mass = 1000.f;


	FVector Velocity;




	UPROPERTY(EditAnywhere)
	float ForceMultiplier = 5.f;
	
	// Max Force Applied to the car measured in (N)
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 1000.f;

	void MoveForward(float Value);


	float Throttle;

};
