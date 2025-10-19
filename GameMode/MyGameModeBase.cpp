// Fill out your copyright notice in the Description page of Project Settings.


#include "InvatareCPP/GameMode/MyGameModeBase.h"
#include "MyGameModeBase.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "InvatareCPP/Player/PlayerCharacter.h"

AMyGameModeBase::AMyGameModeBase()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = APlayerController::StaticClass();
}

AActor* AMyGameModeBase::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	TArray<APlayerStart*> SpawnPoints;

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		SpawnPoints.Add(*It);
	}

	if (SpawnPoints.Num() > 0)
	{
		const int32 Index = FMath::RandRange(0, SpawnPoints.Num() - 1);
		return SpawnPoints[Index];
	}

	return Super::FindPlayerStart_Implementation(Player, IncomingName);
}

void AMyGameModeBase::RespawnPlayer(AController* PlayerToRespawn)
{
	if (!PlayerToRespawn) return;

	APawn** DeadPawnPtr = DeadPawnMap.Find(PlayerToRespawn);
	if (!DeadPawnPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("RespawnPlayer: Nu am gasit pionul in DeadPawnMap!"));
		return;
	}

	APlayerCharacter* DeadCharacter = Cast<APlayerCharacter>(*DeadPawnPtr);

	DeadPawnMap.Remove(PlayerToRespawn);

	AActor* SpawnPoint = FindPlayerStart(PlayerToRespawn);
	if (!SpawnPoint) 
	{
		UE_LOG(LogTemp, Error, TEXT("RespawnPlayer: Nu am gasit niciun PlayerStart in nivel!"));
		return;
	}

	FTransform SpawnTransform = SpawnPoint->GetActorTransform();

	if (DeadCharacter)
	{
		DeadCharacter->SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::TeleportPhysics);

		DeadCharacter->ResetHealth();

		DeadCharacter->Multicast_HandleRespawn();
		PlayerToRespawn->Possess(DeadCharacter);
	}
}

void AMyGameModeBase::PlayerDied(AController* PlayerController, APawn* DeadPawn)
{
	if (!PlayerController || !DeadPawn) return;

	DeadPawnMap.Add(PlayerController, DeadPawn);

	float RespawnDelay = 5.0f;

	FTimerDelegate RespawnDelegate;

	RespawnDelegate.BindUObject(this, &AMyGameModeBase::RespawnPlayer, PlayerController);

	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);
}
