#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/UserDefinedEnum.h"
#include "Sound/SoundBase.h"
#include "Engine/TimerHandle.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "BaseWeapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    EWT_Pistol  UMETA(DisplayName = "Pistol"),
    EWT_Rifle   UMETA(DisplayName = "Rifle"),
    EWT_Knife   UMETA(DisplayName = "Knife")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChangedSignature, int32, CurrentAmmo, int32, MaxAmmo);

UCLASS()
class INVATARECPP_API ABaseWeapon : public AActor
{
    GENERATED_BODY()

public:
    ABaseWeapon();
    void Fire(FVector CameraLocation, FRotator CameraRotation);
    void Reload();

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnAmmoChangedSignature OnAmmoChanged;

    UFUNCTION(Client, Reliable)
    void BroadcastCurrentState();

    FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }


protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
    EWeaponType WeaponType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
    float Damage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
    int32 MaxAmmo;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
    float WeaponRange;

    UPROPERTY(ReplicatedUsing = OnRep_CurrentAmmo)
    int32 CurrentAmmo;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Properties")
    float FireRate; 

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Properties")
    float ReloadTime;

    UPROPERTY(Replicated)
    bool bIsReloading;

    UPROPERTY(Replicated)
    bool bCanFire;

    FTimerHandle FireRateTimerHandle;

    FTimerHandle TimerHandle_FinishReload;

    UFUNCTION()
    void ResetFire(); 

    UFUNCTION()
    void FinishReload();

    UFUNCTION(Server, Reliable)
    void Server_Reload();

    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_PlayFireEffects();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Mesh")
    USkeletalMeshComponent* WeaponMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    USoundBase* HitSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
    UAnimMontage* FireAnimMontage_FP; 

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
    UAnimMontage* FireAnimMontage_TP; 

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
    UAnimMontage* ReloadAnimMontage_FP; 

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
    UAnimMontage* ReloadAnimMontage_TP; 

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
    UNiagaraSystem* MuzzleFlashEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
    FName MuzzleSocketName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
    USoundBase* FireSound;

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayHitSound();

    UFUNCTION()
    void OnRep_CurrentAmmo();

};