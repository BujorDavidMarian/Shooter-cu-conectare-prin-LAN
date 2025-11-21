// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "InvatareCPP/SaveGame/MySaveGame.h"
#include "MyGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCrosshairChangedSignature, UTexture2D*, NewCrosshairTexture);

/**
 * 
 */
UCLASS()
class INVATARECPP_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category ="Settings")
	void SetCrosshairIndex(int32 NewIndex);

	UFUNCTION(BlueprintPure, Category = "Settings")
	UTexture2D* GetCurrentCrosshairTexture() const;

	UFUNCTION(BlueprintPure, Category = "Settings")
	int32 GetCurrentCrosshairIndex() const { return CurrentCrosshairIndex; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TArray<UTexture2D*> AvailableCrosshairs;

	UPROPERTY(BlueprintAssignable, Category = "Settings")
	FOnCrosshairChangedSignature OnCrosshairChanged;

	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	FString PlayerName;


protected:

	UPROPERTY()
	int32 CurrentCrosshairIndex = 0;

	void SaveGame();
	void LoadGame();

	const FString SaveSlotName = "GameSettings";
	
};
