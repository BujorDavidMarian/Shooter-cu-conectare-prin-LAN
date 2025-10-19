// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Containers/Map.h"
#include "MyGameModeBase.generated.h"

/**
 * 
 */

class APlayerCharacter;
class APawn;

UCLASS()
class INVATARECPP_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyGameModeBase();
	void RespawnPlayer(AController* PlayerToRespawn);
	void PlayerDied(AController* PlayerController, APawn* DeadPawn);

protected:

	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName = TEXT("")) override;

private: 

	FTimerHandle RespawnTimerHandle;

	UPROPERTY()
	TMap<AController*, APawn*> DeadPawnMap;
	
};
