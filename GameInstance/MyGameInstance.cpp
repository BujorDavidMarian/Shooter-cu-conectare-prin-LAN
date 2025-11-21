// Fill out your copyright notice in the Description page of Project Settings.


#include "InvatareCPP/GameInstance/MyGameInstance.h"


void UMyGameInstance::Init()
{
	Super::Init();
	LoadGame();
}

void UMyGameInstance::SetCrosshairIndex(int32 NewIndex)
{
	GEngine->AddOnScreenDebugMessage( - 1, 5.0f, FColor::Cyan, TEXT("SetCrosshair") );
	if (AvailableCrosshairs.IsValidIndex(NewIndex))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Idex Valid"));

		CurrentCrosshairIndex = NewIndex;

		OnCrosshairChanged.Broadcast(GetCurrentCrosshairTexture());

		SaveGame();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Idex not Valid"));

	}
}

UTexture2D* UMyGameInstance::GetCurrentCrosshairTexture() const
{
	if (AvailableCrosshairs.IsValidIndex(CurrentCrosshairIndex))
	{
		return AvailableCrosshairs[CurrentCrosshairIndex];
	}

	return (AvailableCrosshairs.Num() > 0) ? AvailableCrosshairs[0] : nullptr;
}


void UMyGameInstance::SaveGame()
{

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("SAVE!"));

	UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

	if (SaveGameInstance)
	{
		SaveGameInstance->SavedCrosshairIndex = CurrentCrosshairIndex;
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, 0);
	}
}

void UMyGameInstance::LoadGame()
{

	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		UMySaveGame* LoadGameInstance = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
		if (LoadGameInstance)
		{
			CurrentCrosshairIndex = LoadGameInstance->SavedCrosshairIndex;
		}
	}
}

