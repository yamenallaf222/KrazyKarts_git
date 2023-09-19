// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		NetUpdateFrequency = 1;
	}
	
}

void AGoKart::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGoKart, ServerState);
	DOREPLIFETIME(AGoKart, Throttle);
	DOREPLIFETIME(AGoKart, SteeringThrow);
}
void AGoKart::OnRep_ServerState()
{
	SetActorTransform(ServerState.Transform);
	Velocity = ServerState.Velocity;
}



// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsLocallyControlled())
	{
		FGoKartMove Move;
		Move.Throttle = Throttle;
		Move.SteeringThrow = SteeringThrow;
		Move.DeltaTime = DeltaTime;
		//TODO : Set Move.Time
		Server_SendMove(Move);
	}


	FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle;
	
	Force += GetAirResistance();
	
	Force += GetRollResistance();

	FVector Acceleration = Force / Mass;

	Velocity = Velocity + Acceleration * DeltaTime;
	

	UpdateRotationInRegardsToVelocity(DeltaTime);

	UpdateLocationFromVelocity(DeltaTime);

	if (HasAuthority())
	{
		ServerState.Transform = GetActorTransform();
		ServerState.Velocity = Velocity;
	}

	
	DrawDebugString(GetWorld(), FVector{ 0 , 0 ,200 }, GetTextRole(GetLocalRole()), this, FColor::White, DeltaTime);

}

FString AGoKart::GetTextRole(ENetRole ActorRole)
{
	switch (ActorRole)
	{
	case ROLE_Authority: 
		return "Authority";
	case ROLE_SimulatedProxy: 
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	default: 
		return "Error";
	}
}


// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AGoKart::Local_MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AGoKart::Local_MoveRight);
}


void AGoKart::Server_SendMove_Implementation(FGoKartMove Move)
{
	Throttle = Move.Throttle ;
	SteeringThrow = Move.SteeringThrow;
}

bool AGoKart::Server_SendMove_Validate(FGoKartMove Move)
{
	return true; //TODO make a better validation
}



void AGoKart::Local_MoveForward(float Value)
{
	Throttle = Value;
}

void AGoKart::Local_MoveRight(float Value)
{
	SteeringThrow = Value;
}


FVector AGoKart::GetAirResistance()
{
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}


FVector AGoKart::GetRollResistance()
{
	//dividing by 100 to convert to meters
	float AccelerationDueToGravity = - GetWorld()->GetGravityZ() / 100;

	float NormalForce = Mass * AccelerationDueToGravity;

	return   -Velocity.GetSafeNormal() * RRCoefficient * NormalForce;
}


void AGoKart::UpdateLocationFromVelocity(float DeltaTime)
{

	FVector Translation = Velocity * 100 * DeltaTime;

	FHitResult HitResult;

	AddActorWorldOffset(Translation, true , &HitResult);



	
	if (HitResult.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}	

}



void AGoKart::UpdateRotationInRegardsToVelocity(float DeltaTime)
{
	float DeltaLocation = FVector::DotProduct(GetActorForwardVector() , Velocity) * DeltaTime;

	//Radians is the unit   , this based on the formula dx = dtheta * radius
	float RotationAngle = DeltaLocation / MinTurningRadius * SteeringThrow;

	FQuat RotationDeltaTime(GetActorUpVector(), RotationAngle);
	
	Velocity = RotationDeltaTime.RotateVector(Velocity);

	AddActorWorldRotation(RotationDeltaTime);
}


