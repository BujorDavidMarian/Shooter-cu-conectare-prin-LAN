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
    WeaponRange = 10000.0f;
    CurrentAmmo = MaxAmmo;

    FireRate = 0.2f;
    ReloadTime = 1.5f;
    bCanFire = true;
    bIsReloading = false;
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ABaseWeapon, CurrentAmmo);
}

void ABaseWeapon::OnRep_CurrentAmmo()
{
    OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
}

void ABaseWeapon::Multicast_PlayHitSound_Implementation()
{
    if (HitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),			
            HitSound,			
            GetActorLocation()	
        );
    }
}

void ABaseWeapon::Fire(FVector CameraLocation, FRotator CameraRotation)
{
    if (!HasAuthority() || CurrentAmmo <= 0 || !bCanFire || bIsReloading)
    {
        return;
    }
        
    CurrentAmmo--;

    OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);

    bCanFire = false;

    FHitResult HitResult;

    FVector End = CameraLocation + CameraRotation.Vector() * WeaponRange;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(GetOwner());

    if (GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, End, ECC_Visibility, Params))
    {
        AActor* HitActor = HitResult.GetActor();
        if (HitActor)
        {
           if (APlayerCharacter* HitPlayer = Cast<APlayerCharacter>(HitActor))
           {
               Multicast_PlayHitSound();
               HitPlayer->Server_TakeDamage(Damage);
           }
            // Daca ai lovit altceva 
        }
    }

    Multicast_PlayFireEffects();

    GetWorld()->GetTimerManager().SetTimer(
        FireRateTimerHandle, 
        this,                   
        &ABaseWeapon::ResetFire,
        FireRate,       
        false                   
    );
}

void ABaseWeapon::Reload()
{
    Server_Reload();
}

void ABaseWeapon::Server_Reload_Implementation()
{
    if (bIsReloading || CurrentAmmo < MaxAmmo)
    {
        CurrentAmmo = MaxAmmo;
    }
    OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);

    bIsReloading = true;

    if (APlayerCharacter* OwningCharacter = Cast<APlayerCharacter>(GetOwner()))
    {
        OwningCharacter->SetIsReloading_Anim(true);
    }

    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle_FinishReload,
        this,
        &ABaseWeapon::FinishReload, 
        ReloadTime,
        false
    );
}

void ABaseWeapon::Multicast_PlayFireEffects_Implementation()
{
    //efecte de particule, sunete, etc.
    if (UAnimInstance* AnimInstance = WeaponMesh->GetAnimInstance())
    {
        AnimInstance->Montage_Play(FireAnimMontage_FP);
        
    }

    if (MuzzleFlashEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAttached(
            MuzzleFlashEffect,      
            WeaponMesh,             
            MuzzleSocketName,       
            FVector::ZeroVector,   
            FRotator::ZeroRotator,  
            EAttachLocation::SnapToTargetIncludingScale, 
            true                    
        );
    }

    if (FireSound)
    {
        UGameplayStatics::SpawnSoundAttached(
            FireSound,             
            WeaponMesh,            
            MuzzleSocketName,
            FVector::ZeroVector,   
            FRotator::ZeroRotator, 
            EAttachLocation::SnapToTarget, 
            true                 
        );
    }
}

void ABaseWeapon::BroadcastCurrentState_Implementation()
{
    OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
}

void ABaseWeapon::ResetFire()
{
    bCanFire = true;
}

void ABaseWeapon::FinishReload()
{
    bIsReloading = false;
    if (APlayerCharacter* OwningCharacter = Cast<APlayerCharacter>(GetOwner()))
    {
        OwningCharacter->SetIsReloading_Anim(false); 
    }
}
