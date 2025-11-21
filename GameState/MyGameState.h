// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnKillSignature, const FString&, KillerName, const FString&, VictimName);


UCLASS()
class INVATARECPP_API AMyGameState : public AGameState
{

    GENERATED_BODY()
    
public:

    UPROPERTY(BlueprintAssignable, Category = "Game")
    FOnKillSignature OnKillOccurred;

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_BroadcastKill(const FString& KillerName, const FString& VictimName);
};
