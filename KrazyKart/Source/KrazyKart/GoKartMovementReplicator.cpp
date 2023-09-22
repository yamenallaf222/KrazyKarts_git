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


FHermiteCubicSpline UGoKartMovementReplicator::CreateSpline()
{
	FHermiteCubicSpline Spline;

	Spline.TargetLocation = ServerState.Transform.GetLocation();

	Spline.StartLocation = ClientStartTransform.GetLocation();

	
	Spline.StartDerivative = ClientStartVelocity * VelocityToDerivative();

	Spline.TargetDerivative = ServerState.Velocity * VelocityToDerivative();

	return Spline;

}


void UGoKartMovementReplicator::InterpolateLocation(const FHermiteCubicSpline& Spline, float LerpRatio)
{
	if (!ensure(MeshOffsetRoot != nullptr))return;
	FVector OwnerNewLerpToLocation = Spline.InterpolateLocation(LerpRatio);
	MeshOffsetRoot->SetWorldLocation(OwnerNewLerpToLocation);
}

void UGoKartMovementReplicator::InterpolateVelocity(const FHermiteCubicSpline& Spline, float LerpRatio)
{
	if (!ensure(GoKartMovementComponent != nullptr))return;


	FVector InterpolatedVelocity = Spline.InterpolateDerivative(LerpRatio) / VelocityToDerivative();

	GoKartMovementComponent->SetVelocityValue(InterpolatedVelocity);

}
void UGoKartMovementReplicator::InterpolateRotation(float LerpRatio)
{
	if (!ensure(MeshOffsetRoot != nullptr))return;

	FQuat TargetRotation = ServerState.Transform.GetRotation();

	double SlerpRatio = LerpRatio;

	FQuat StartRotation = ClientStartTransform.GetRotation();

	FQuat OwnerNewSlerpToRotation = FQuat::Slerp(StartRotation, TargetRotation, SlerpRatio);

	MeshOffsetRoot->SetWorldRotation(OwnerNewSlerpToRotation);
}

float UGoKartMovementReplicator::VelocityToDerivative()
{
	//Multiplied by 100 for converting to centimeters as we further multiply it with velocity as unreal uses such measure
	float VelocityToDerivative = ClientTimeBetweenLastUpdates * 100;

	return VelocityToDerivative;
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
		ClientTimeSinceUpdate += DeltaTime;



		if (ClientTimeBetweenLastUpdates < KINDA_SMALL_NUMBER) return;

		

		float LerpRatio = ClientTimeSinceUpdate / ClientTimeBetweenLastUpdates;

		FHermiteCubicSpline Spline = CreateSpline();

		
		

		InterpolateLocation(Spline, LerpRatio);

		InterpolateVelocity(Spline, LerpRatio);

		InterpolateRotation(LerpRatio);


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

	 SimulatedTimeOfMovesOnClient += Move.DeltaTime;
	GoKartMovementComponent->SimulateMove(Move);

	UpdateServerState(Move);
}





bool UGoKartMovementReplicator::Server_SendMove_Validate(FGoKartMove Move)
{
	float ProposedTime = SimulatedTimeOfMovesOnClient + Move.DeltaTime;
	bool bClinetNotRunningAhead = ProposedTime < GetWorld()->TimeSeconds;
	if (!bClinetNotRunningAhead)
	{
		UE_LOG(LogTemp, Error, TEXT("Client is Running Too Fast , Cheating!!!"));
		return false;
	}

	if (!Move.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Server Received invalid Moves!!!"));
		return false;
	}

	return true ;
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
	switch (GetOwnerRole())
	{
		case ROLE_AutonomousProxy : AutonomousProxy_OnRep_ServerState();
			break;
		case ROLE_SimulatedProxy: SimulatedProxy_OnRep_ServerState();
			break;
		default :
			break;
	}

}

void UGoKartMovementReplicator::AutonomousProxy_OnRep_ServerState()
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

void UGoKartMovementReplicator::SimulatedProxy_OnRep_ServerState()
{
	if (!ensure(GoKartMovementComponent != nullptr))return;


	ClientTimeBetweenLastUpdates = ClientTimeSinceUpdate;

	ClientTimeSinceUpdate = 0;
	

	if (!ensure(MeshOffsetRoot != nullptr))return;

	ClientStartTransform.SetLocation(MeshOffsetRoot->GetComponentLocation());
	ClientStartTransform.SetRotation(MeshOffsetRoot->GetComponentQuat());

	ClientStartVelocity = GoKartMovementComponent->GetVelocityValue();
	
	GetOwner()->SetActorTransform(ServerState.Transform);
}
