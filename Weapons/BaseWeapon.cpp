#include "InvatareCPP/Weapons/BaseWeapon.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "InvatareCPP/Player/PlayerCharacter.h"
#include "GameFramework/Character.h"

ABaseWeapon::ABaseWeapon()
{
    bReplicates = true;
    PrimaryActorTick.bCanEverTick = false;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    RootComponent = WeaponMesh;
    WeaponMesh->CastShadow = false;

    Damage = 20.0f;
    MaxAmmo = 30;
    CurrentAmmo = MaxAmmo;
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ABaseWeapon, CurrentAmmo);
}

void ABaseWeapon::Fire(FVector CameraLocation, FRotator CameraRotation)
{
    if (!HasAuthority() || CurrentAmmo <= 0)
    {
           return;
           GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Dose not have authority"));
    }
        
    CurrentAmmo--;

    FHitResult HitResult;

    FVector End = CameraLocation + CameraRotation.Vector() * 10000.0f;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(GetOwner());

    if (GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, End, ECC_Visibility, Params))
    {
        AActor* HitActor = HitResult.GetActor();
        if (HitActor)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("HitSomething!"));
           if (APlayerCharacter* HitPlayer = Cast<APlayerCharacter>(HitActor))
           {
               GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("HitPlayer"));
               HitPlayer->Server_TakeDamage(Damage);
           }
            // Daca ai lovit altceva 
        }
    }

    Multicast_PlayFireEffects();
}

void ABaseWeapon::Reload()
{
    Server_Reload();
}

void ABaseWeapon::Server_Reload_Implementation()
{
    if (CurrentAmmo < MaxAmmo)
    {
        CurrentAmmo = MaxAmmo;
    }
}

void ABaseWeapon::Multicast_PlayFireEffects_Implementation()
{
    //efecte de particule, sunete, etc.
}