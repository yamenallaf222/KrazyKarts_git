// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"




// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	MovementComponent = CreateDefaultSubobject<UGoKartMovementComponent>(TEXT("MovementComponent"));
	MovementReplicator = CreateDefaultSubobject<UGoKartMovementReplicator>(TEXT("MovementReplicator"));


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





// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);




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





void AGoKart::Local_MoveForward(float Value)
{
	if (!ensure(MovementComponent != nullptr))return;

	MovementComponent->SetThrottleValue(Value);
}

void AGoKart::Local_MoveRight(float Value)
{
	if (!ensure(MovementComponent != nullptr))return;

	MovementComponent->SetSteeringThrowValue(Value);
}

