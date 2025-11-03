// Fill out your copyright notice in the Description page of Project Settings.


#include "InvatareCPP/Heal/Heal.h"
#include "InvatareCPP/Player/PlayerCharacter.h"

AHeal::AHeal()
{
    PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionBox->SetupAttachment(Mesh); 

    CollisionBox->SetCollisionProfileName(TEXT("Trigger"));

}

void AHeal::BeginPlay()
{
	Super::BeginPlay();

    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AHeal::OnOverlapBegin);
	
}

void AHeal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    if (OtherActor && (OtherActor != this))
    {
        if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
        {
            Player->Heal(HealAmount);

            Mesh->SetVisibility(false);

            CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

            GetWorld()->GetTimerManager().SetTimer(
                RespawnTimerHandle,     
                this,                   
                &AHeal::RespawnPickup, 
                RespawnTime,            
                false                   
            );
        }
    }
}

void AHeal::RespawnPickup()
{
    GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);

    Mesh->SetVisibility(true);

    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}



