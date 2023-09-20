// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovementComponent.h"
#include "GoKartMovementReplicator.generated.h"

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


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAZYKART_API UGoKartMovementReplicator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMovementReplicator();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		

private:

	UPROPERTY(VisibleAnywhere)
	UGoKartMovementComponent* GoKartMovementComponent;


	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState  ServerState;


	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FGoKartMove Move);


	UFUNCTION()
	void OnRep_ServerState();


	void ClearAcknowledgedMoves(FGoKartMove LastMove);


	void UpdateServerState(const FGoKartMove& Move);

	TArray<FGoKartMove> UnacknowledgedMoves;

};