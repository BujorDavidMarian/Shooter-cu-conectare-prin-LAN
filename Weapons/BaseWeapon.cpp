#include "InvatareCPP/Weapons/BaseWeapon.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "InvatareCPP/Player/PlayerCharacter.h"
#include "DrawDebugHelpers.h"
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

    FireRate = 0.2f;
    ReloadTime = 1.5f;
    bCanFire = true;
    bIsReloading = false;
}

void ABaseWeapon::BeginPlay()
{
    Super::BeginPlay(); 

    CurrentAmmo = MaxAmmo;
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

        if (CurrentAmmo <= 0)
        {
            Multicast_PlayDryFireEffects();
        }

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

    Params.bReturnPhysicalMaterial = true;

    if (GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, End, ECC_GameTraceChannel1, Params))
    {
        /*DrawDebugLine(
            GetWorld(),
            CameraLocation, // Start
            End,            // End
            FColor::Red,    // Culoare
            false,          // Persistent
            5.0f,           // Timp (secunde)
            0,              // Depth
            1.0f            // Grosime
        );

        if (HitResult.bBlockingHit)
        {
            DrawDebugPoint(
                GetWorld(),
                HitResult.ImpactPoint,
                20.0f,
                FColor::Green,
                false,
                5.0f
            );

            if (GEngine && HitResult.GetComponent())
            {
                FString Msg = FString::Printf(TEXT("Am lovit componenta: %s"), *HitResult.GetComponent()->GetName());
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Msg);
            }
        }*/

        AActor* HitActor = HitResult.GetActor();
        if (HitActor)
        {
           if (APlayerCharacter* HitPlayer = Cast<APlayerCharacter>(HitActor))
           {
               float DamageMultiplier = 1.0f;

               UPhysicalMaterial* HitPhysMaterial = HitResult.PhysMaterial.Get();

               if (HitPhysMaterial)
               {
                   EPhysicalSurface SurfaceType = HitPhysMaterial->SurfaceType;
                   switch (SurfaceType)
                   {
                   case EPhysicalSurface::SurfaceType1: 
                       DamageMultiplier = 4.0f;
                       UE_LOG(LogTemp, Warning, TEXT("Hit HEAD!"));
                       break;
                   case EPhysicalSurface::SurfaceType2: 
                       DamageMultiplier = 1.5f;
                       UE_LOG(LogTemp, Warning, TEXT("Hit BODY!"));
                       break;
                   case EPhysicalSurface::SurfaceType3: 
                       DamageMultiplier = 0.8f;
                       UE_LOG(LogTemp, Warning, TEXT("Hit LIMB!"));
                       break;
                   default:
                       DamageMultiplier = 1.0f;
                       break;
                   }
               }

               float FinalDamage = Damage * DamageMultiplier;

               APawn* OwnerPawn = Cast<APawn>(GetOwner());

               if (OwnerPawn)
               {
                   AController* InstigatorController = OwnerPawn->GetController();

                   if (InstigatorController)
                   {
                       HitPlayer->Server_TakeDamage(FinalDamage, InstigatorController);
                   }
               }
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
    if (CurrentAmmo == MaxAmmo)
    {
        return;
    }

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

    Multicast_PlayRealoadEffects();
}

void ABaseWeapon::Multicast_PlayFireEffects_Implementation()
{

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

void ABaseWeapon::Multicast_PlayRealoadEffects_Implementation()
{
    if (RealoadSound)
    {
        UGameplayStatics::SpawnSoundAttached(
            RealoadSound,
            WeaponMesh,
            MuzzleSocketName,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::SnapToTarget,
            true
        );
    }

}

void ABaseWeapon::Multicast_PlayDryFireEffects_Implementation()
{
    if (DryFireSound)
    {
        UGameplayStatics::SpawnSoundAttached(
            DryFireSound,
            WeaponMesh,
            MuzzleSocketName,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::SnapToTarget,
            true
        );
    }
}
