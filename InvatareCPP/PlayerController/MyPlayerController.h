// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class INVATARECPP_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Network")
	void HostGame(FName MapName);

	UFUNCTION(BlueprintCallable, Category = "Network")
	void JoinGame(const FString& IpAddress);
	
};
