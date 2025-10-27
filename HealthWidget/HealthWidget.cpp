// Fill out your copyright notice in the Description page of Project Settings.


#include "InvatareCPP/HealthWidget/HealthWidget.h"

void UHealthWidget::UpdateHealth(float Percent)
{
    if (HealthBar && HealthText)
    {
        HealthBar->SetPercent(Percent);
        HealthText->SetText(FText::FromString(FString::Printf(TEXT("%d"), (int32)(Percent * 100.0f))));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HealthBar component is nullptr in UHealthWidget!"));
    }
}
