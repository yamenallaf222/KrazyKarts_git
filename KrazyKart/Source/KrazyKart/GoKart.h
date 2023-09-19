// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

USTRUCT()
struct FGoKartMove
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	float Throttle;
	
	UPROPERTY()
	float SteeringThrow;

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	float Time;

};

USTRUCT()
struct FGoKartState
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	FTransform Transform;
	

	UPROPERTY()
	FVector Velocity;


	UPROPERTY()
	FGoKartMove LastMove;
};




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
	void Server_SendMove(FGoKartMove Move);

	void Local_MoveForward(float Value);

	void Local_MoveRight(float Value);


	FString GetTextRole(ENetRole);


	FVector Velocity;


	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState  ServerState;	
	
	


	UFUNCTION()
	void OnRep_ServerState();	
	
	UPROPERTY(Replicated)
	float Throttle;

	UPROPERTY(Replicated)
	float SteeringThrow;

};
