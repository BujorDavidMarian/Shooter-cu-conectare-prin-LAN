// Fill out your copyright notice in the Description page of Project Settings.


#include "InvatareCPP/PlayerController/MyPlayerController.h"
#include "Kismet/GameplayStatics.h"


void AMyPlayerController::HostGame(FName MapName)
{
	// Host + listen server
	UGameplayStatics::OpenLevel(GetWorld(), MapName, true, "listen");
}

void AMyPlayerController::JoinGame(const FString& IpAddress)
{
	ClientTravel(IpAddress, TRAVEL_Absolute);
}
