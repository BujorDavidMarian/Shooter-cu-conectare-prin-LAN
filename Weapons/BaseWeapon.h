#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UCLASS()
class INVATARECPP_API ABaseWeapon : public AActor
{
    GENERATED_BODY()

public:
    ABaseWeapon();
    void Fire(FVector CameraLocation, FRotator CameraRotation);
    void Reload();

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
    float Damage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Properties")
    int32 MaxAmmo;

    UPROPERTY(Replicated)
    int32 CurrentAmmo;

    UFUNCTION(Server, Reliable)
    void Server_Reload();

    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_PlayFireEffects();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Mesh")
    USkeletalMeshComponent* WeaponMesh;
};