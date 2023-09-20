// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKartMovementReplicator.h"
#include "Net/UnrealNetwork.h"
// Sets default values for this component's properties
UGoKartMovementReplicator::UGoKartMovementReplicator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	SetIsReplicated(true);

	
	

}


// Called when the game starts
void UGoKartMovementReplicator::BeginPlay()
{
	Super::BeginPlay();

	GoKartMovementComponent = GetOwner()->FindComponentByClass<UGoKartMovementComponent>();
	// ...
	
}

void UGoKartMovementReplicator::UpdateServerState(const FGoKartMove& Move)
{
	ServerState.LastMove = Move;

	ServerState.Transform = GetOwner()->GetActorTransform();

	ServerState.Velocity = GoKartMovementComponent->GetVelocityValue();
}



// Called every frame
void UGoKartMovementReplicator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (!ensure(GoKartMovementComponent != nullptr))return;
	FGoKartMove LastMove = GoKartMovementComponent->GetLastMove();

	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy && GoKartMovementComponent)
	{
		
		UnacknowledgedMoves.Add(LastMove);
		UE_LOG(LogTemp, Warning, TEXT("The queue length is %d"), UnacknowledgedMoves.Num());
		Server_SendMove(LastMove);
	}


	//if we are the server and we are in control of the pawn
	if (GetOwner()->GetLocalRole() == ROLE_Authority && GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
	{
		if (GoKartMovementComponent)
		{

			UpdateServerState(LastMove);

		}
	}

	if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		GoKartMovementComponent->SimulateMove(ServerState.LastMove);
	}



	// ...
}


void UGoKartMovementReplicator::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGoKartMovementReplicator, ServerState);

}




void UGoKartMovementReplicator::Server_SendMove_Implementation(FGoKartMove Move)
{
	if (!ensure(GoKartMovementComponent != nullptr))return;


	GoKartMovementComponent->SimulateMove(Move);

	UpdateServerState(Move);
}



void UGoKartMovementReplicator::ClearAcknowledgedMoves(FGoKartMove LastMove)
{

	TArray<FGoKartMove> NewMoves;

	for (const FGoKartMove& Move : UnacknowledgedMoves)
	{
		if (Move.Time > LastMove.Time)
		{
			NewMoves.Add(Move);
		}
	}

	UnacknowledgedMoves = NewMoves;
}





void UGoKartMovementReplicator::OnRep_ServerState()
{

	if (!ensure(GoKartMovementComponent != nullptr))return;


	GetOwner()->SetActorTransform(ServerState.Transform);

	GoKartMovementComponent->SetVelocityValue(ServerState.Velocity);

	ClearAcknowledgedMoves(ServerState.LastMove);

	for (const FGoKartMove& Move : UnacknowledgedMoves)
	{

		GoKartMovementComponent->SimulateMove(Move);
	}
}



bool UGoKartMovementReplicator::Server_SendMove_Validate(FGoKartMove Move)
{
	return true; //TODO make a better validation
}
