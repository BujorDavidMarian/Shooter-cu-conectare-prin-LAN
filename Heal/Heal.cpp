// Fill out your copyright notice in the Description page of Project Settings.


#include "InvatareCPP/Heal/Heal.h"
#include "InvatareCPP/Player/PlayerCharacter.h"

AHeal::AHeal()
{
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;

    SetReplicateMovement(false);
    bNetLoadOnClient = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionBox->SetupAttachment(Mesh); 

    CollisionBox->SetCollisionProfileName(TEXT("Trigger"));

}

void AHeal::BeginPlay()
{
	Super::BeginPlay();

    if (HasAuthority())
    {
        CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AHeal::OnOverlapBegin);
    }
	
}

void AHeal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority())
    {
        return;
    }

    if (OtherActor && (OtherActor != this))
    {
        if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
        {
            Player->Heal(HealAmount);

            Multicast_DisablePickup();

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

void AHeal::Multicast_DisablePickup_Implementation()
{
    Mesh->SetVisibility(false);
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AHeal::RespawnPickup()
{
    if (!HasAuthority())
    {
        return;
    }

    GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);

    Multicast_EnablePickup();
}

void AHeal::Multicast_EnablePickup_Implementation()
{
    Mesh->SetVisibility(true);
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}



