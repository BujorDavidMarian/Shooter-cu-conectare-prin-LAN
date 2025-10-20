
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InvatareCPP/HealthWidget/HealthWidget.h"
#include "Components/SphereComponent.h"
#include "InvatareCPP/Weapons/BaseWeapon.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class INVATARECPP_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HandleRespawn();

	void ResetHealth();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_Controller() override;

	virtual void UnPossessed() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Client, Reliable)
	void Client_AttachCameraToRagdoll();

	UPROPERTY()
	USceneComponent* OriginalCameraParent;

	FTransform OriginalCameraTransform;

	bool bOriginalUsePawnControlRotation;

private:
	void Client_SaveCameraDefaults();

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	UPROPERTY(EditAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)

	class USkeletalMeshComponent* FpArms;

	void MoveForward(float Input);

	void MoveRight(float Input);

	void TurnCamera(float Input);

	void LookUp(float Input);

	void StartCrouch();

	void StopCrouch();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed;

	UFUNCTION(Server, Reliable)
	void Server_SetSprinting(bool bNewSprinting);

	bool bIsSprinting = false;

	void StartSprinting();

	void StopSprinting();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sliding")
	float MinSlideSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sliding")
	float SlideFriction;

	float DefaultBrakingDeceleration;

	float DefaultGroundFriction;

	virtual void Crouch(bool bClientSimulation = false) override;

	virtual void UnCrouch(bool bClientSimulation = false) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_IsSliding, Category = "Sliding")
	bool bIsSliding;

	UFUNCTION()
	void OnRep_IsSliding();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetSliding(bool bNewSliding);

	void StartSlide_Internal();
	void StopSlide_Internal();

	FTimerHandle SlideGraceTimerHandle;

	bool bCanSlideAfterSprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sliding")
	float SlideGraceTime;

	void OnSlideGraceTimerEnd();

	//HUD
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UHealthWidget> HealthWidgetClass;

	UPROPERTY()
	UHealthWidget* HealthWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UUserWidget> DamageEffectWidgetClass;

	UPROPERTY()
	UUserWidget* DamageEffectWidget;

	//Viata
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	float MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_HealthChanged, VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth;

	UPROPERTY()
	float LastHealth;

	UFUNCTION()
	void OnRep_HealthChanged();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDeath();

	UFUNCTION()
	void TakeDamage(float Damage);

public:

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	bool IsDead() const;


	//Take damage on server
	UFUNCTION(Server, Reliable)
	void Server_TakeDamage(float DamageAmount);

	// Helper pentru a accesa viata actuala
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	//Move head and spine with camera

	UPROPERTY(ReplicatedUsing = OnRep_AimRotation)
	FRotator AimRotation_Replicated;

	UPROPERTY(Replicated) 
	FVector LookAtLocation_Replicated;

	UFUNCTION(Server, Reliable)
	void Server_SetLookAtLocation(FVector NewLocation);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* LookAtTargetSphere;

public:

	UFUNCTION(BlueprintCallable, Category = "Animation")
	FVector GetLookAtLocation() const { return LookAtLocation_Replicated; }

	UFUNCTION()
	void OnRep_AimRotation();

//Weapons

public:

	void EquipWeapon(ABaseWeapon* WeaponToEquip);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<ABaseWeapon> DefaultWeaponClass;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon)
	ABaseWeapon* CurrentWeapon;

	UFUNCTION()
	void OnRep_CurrentWeapon();

	void StartFire();
	void StartReload();

	UFUNCTION(Server, Reliable)
	void Server_StartFire();

};
