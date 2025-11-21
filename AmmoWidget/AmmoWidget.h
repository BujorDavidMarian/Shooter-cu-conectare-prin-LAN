// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "AmmoWidget.generated.h"

/**
 * 
 */
UCLASS()
class INVATARECPP_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()

protected: 

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoText;

public:

	UFUNCTION(BlueprintCallable)
	void UpdateAmmoCount(int32 CurrentAmmo, int32 MaxAmmo);
	
};
