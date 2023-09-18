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




	FVector Velocity;

	FVector GetAirResistance();

	FVector GetRollResistance();

	void UpdateLocationFromVelocity(float DeltaTime);

	void UpdateRotationInRegardsToVelocity(float DeltaTime);




	//mass of the car in kg 
	UPROPERTY(EditAnywhere)
		float Mass = 1000.f;

	//Higher means more drag ** this measure is unitless though the formula used for air resistance is not physics acurate and in fact the air resistance unit is ((Kg/m)/s^2) or (N) **
	UPROPERTY(EditAnywhere)
		float DragCoefficient = 16;

	//Higher means more Roll Resistance ** this measure is unitless though the formula used for air resistance is not physics acurate and in fact the air resistance unit is ((Kg/m)/s^2) or (N) **
	UPROPERTY(EditAnywhere)
		float RRCoefficient = 0.015f;

	// Max Force Applied to the car measured in (N)
	UPROPERTY(EditAnywhere)
		float MaxDrivingForce = 10000.f;


	//Car Cicling Radius in meters
	UPROPERTY(EditAnywhere)
		float MinTurningRadius = 10.f;



	UFUNCTION(Server, Reliable, WithValidation)
		void Server_MoveForward(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_MoveRight(float Value);

	void Local_MoveForward(float Value);

	void Local_MoveRight(float Value);


	FString GetTextRole(ENetRole);



	float Throttle;


	float SteeringThrow;

};
