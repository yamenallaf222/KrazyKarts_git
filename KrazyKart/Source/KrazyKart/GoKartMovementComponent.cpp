// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKartMovementComponent.h"

// Sets default values for this component's properties
UGoKartMovementComponent::UGoKartMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGoKartMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGoKartMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy || GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
	{
		LastMove = CreateMove(DeltaTime);
		SimulateMove(LastMove);
	}

}

FGoKartMove UGoKartMovementComponent::CreateMove(float DeltaTime)
{
	FGoKartMove Move;
	Move.Throttle = Throttle;
	Move.SteeringThrow = SteeringThrow;
	Move.DeltaTime = DeltaTime;
	Move.Time = GetWorld()->TimeSeconds;

	return Move;
}

void UGoKartMovementComponent::SimulateMove(const FGoKartMove& Move)
{
	FVector Force = GetOwner()->GetActorForwardVector() * MaxDrivingForce * Move.Throttle;

	Force += GetAirResistance();

	Force += GetRollResistance();

	FVector Acceleration = Force / Mass;

	Velocity = Velocity + Acceleration * Move.DeltaTime;


	UpdateRotationInRegardsToVelocity(Move.DeltaTime, Move.SteeringThrow);

	UpdateLocationFromVelocity(Move.DeltaTime);

}

FVector UGoKartMovementComponent::GetAirResistance()
{
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}


FVector UGoKartMovementComponent::GetRollResistance()
{
	//dividing by 100 to convert to meters
	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100;

	float NormalForce = Mass * AccelerationDueToGravity;

	return   -Velocity.GetSafeNormal() * RRCoefficient * NormalForce;
}


void UGoKartMovementComponent::UpdateLocationFromVelocity(float DeltaTime)
{ 
	FVector Translation = Velocity * 100 * DeltaTime;

	FHitResult HitResult;

	GetOwner()->AddActorWorldOffset(Translation, true, &HitResult);




	if (HitResult.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}

}


void UGoKartMovementComponent::UpdateRotationInRegardsToVelocity(float DeltaTime, float SteeringThrowOfMove)
{
	float DeltaLocation = FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity) * DeltaTime;

	//Radians is the unit   , this based on the formula dx = dtheta * radius
	float RotationAngle = DeltaLocation / MinTurningRadius * SteeringThrowOfMove;

	FQuat RotationDeltaTime(GetOwner()->GetActorUpVector(), RotationAngle);

	Velocity = RotationDeltaTime.RotateVector(Velocity);

	GetOwner()->AddActorWorldRotation(RotationDeltaTime);

}