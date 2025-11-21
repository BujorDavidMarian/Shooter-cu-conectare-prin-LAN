// Fill out your copyright notice in the Description page of Project Settings.


#include "InvatareCPP/GameState/MyGameState.h"
#include "Net/UnrealNetwork.h"


void AMyGameState::Multicast_BroadcastKill_Implementation(const FString& KillerName, const FString& VictimName)
{
    OnKillOccurred.Broadcast(KillerName, VictimName);
}
