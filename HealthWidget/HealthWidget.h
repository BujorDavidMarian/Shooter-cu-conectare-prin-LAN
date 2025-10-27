// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HealthWidget.generated.h"

/**
 * 
 */
UCLASS()
class INVATARECPP_API UHealthWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthBar;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* HealthText;

public:

    UFUNCTION(BlueprintCallable)
    void UpdateHealth(float Percent);
};
	
