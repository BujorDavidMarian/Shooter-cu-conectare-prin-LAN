// Fill out your copyright notice in the Description page of Project Settings.


#include "InvatareCPP/PlayerController/MyPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"


void AMyPlayerController::HostGame(FName MapName)
{
	UGameplayStatics::OpenLevel(GetWorld(), MapName, true, "listen");
}

void AMyPlayerController::JoinGame(const FString& IpAddress)
{
	ClientTravel(IpAddress, TRAVEL_Absolute);
}

bool AMyPlayerController::Server_SetPlayerName_Validate(const FString& NewName)
{
    if (NewName.IsEmpty() || NewName.Len() > 20)
    {
        return false; 
    }
    return true; 
}

void AMyPlayerController::Server_SetPlayerName_Implementation(const FString& NewName)
{
    if (PlayerState != nullptr)
    {
        PlayerState->SetPlayerName(NewName);
    }
}
