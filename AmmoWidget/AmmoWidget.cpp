// Fill out your copyright notice in the Description page of Project Settings.


#include "InvatareCPP/AmmoWidget/AmmoWidget.h"

void UAmmoWidget::UpdateAmmoCount(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (AmmoText)
	{
		AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo)));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("AmmoText component is nullptr in UAmmoWidget!"));
	}
}
