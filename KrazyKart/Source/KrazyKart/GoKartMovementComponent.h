// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovementComponent.generated.h"


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






UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAZYKART_API UGoKartMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



	void SimulateMove(const FGoKartMove& Move);
private:


	FGoKartMove CreateMove(float DeltaTime);

	
	FVector GetAirResistance();


	FVector GetRollResistance();


	void UpdateLocationFromVelocity(float DeltaTime);


	void UpdateRotationInRegardsToVelocity(float DeltaTime, float SteeringThrowOfMove);

	

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


	FVector Velocity;


	//UPROPERTY(Replicated)
	float Throttle;


	//UPROPERTY(Replicated)
	float SteeringThrow;

	FGoKartMove LastMove;

public:

	float GetThrottleValue()
	{
		return Throttle;
	}

	float GetSteeringThrowValue()
	{
		return SteeringThrow;
	}

	void SetThrottleValue(float ThrottleCopy)
	{
		this->Throttle = ThrottleCopy;
	}

	void SetSteeringThrowValue(float SteeringThrowCopy)
	{
		this->SteeringThrow = SteeringThrowCopy;
	}



	FVector GetVelocityValue()
	{
		return Velocity;
	}

	void SetVelocityValue(FVector VelocityCopy)
	{
		this->Velocity = VelocityCopy;
	}

	FGoKartMove GetLastMove() { return LastMove; }

};
