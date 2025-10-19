// Fill out your copyright notice in the Description page of Project Settings.


#include "InvatareCPP/HealthWidget/HealthWidget.h"

void UHealthWidget::UpdateHealth(float Percent)
{
    if (HealthBar)
    {
        HealthBar->SetPercent(Percent);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HealthBar component is nullptr in UHealthWidget!"));
    }
}
