// Fill out your copyright notice in the Description page of Project Settings.


#include "InvatareCPP/Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/ProgressBar.h"
#include "Components/CapsuleComponent.h"
#include "InvatareCPP/Weapons/BaseWeapon.h"
#include "InvatareCPP/GameMode/MyGameModeBase.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//Camera

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("PLayer Camera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->bUsePawnControlRotation = true;
	
	FpArms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Preson Arms"));
	FpArms->SetOnlyOwnerSee(true);
	FpArms->SetupAttachment(Camera);
	FpArms->CastShadow = false;

	GetMesh()->SetOwnerNoSee(true);

	bReplicates = true;

	LookAtTargetSphere = CreateDefaultSubobject<USphereComponent>(TEXT("LookAtTargetSphere"));

	LookAtTargetSphere->SetupAttachment(Camera);
	LookAtTargetSphere->SetSphereRadius(5.f);
	LookAtTargetSphere->SetHiddenInGame(true);
	LookAtTargetSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);


}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (IsLocallyControlled() && NewController)
	{
		NewController->SetControlRotation(GetActorRotation());

		if (Camera && OriginalCameraParent)
		{
			Camera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			Camera->AttachToComponent(OriginalCameraParent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Camera->SetRelativeTransform(OriginalCameraTransform);
			Camera->bUsePawnControlRotation = bOriginalUsePawnControlRotation;
		}

		APlayerController* Pc = Cast<APlayerController>(NewController);
		if (Pc)
		{
			Pc->EnableInput(Pc);
		}

	}
}

void APlayerCharacter::ResetHealth()
{
	CurrentHealth = MaxHealth;
	LastHealth = MaxHealth;

	OnRep_HealthChanged();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	LastHealth = CurrentHealth;

	if (IsLocallyControlled() && HealthWidgetClass)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());

		if (PlayerController)
		{
			HealthWidget = CreateWidget<UHealthWidget>(PlayerController, HealthWidgetClass);

			if (HealthWidget)
			{
				HealthWidget->AddToViewport();
				HealthWidget->UpdateHealth(GetHealthPercent());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to create HealthWidget!"));
			}
		}
	}

	if (HasAuthority())
	{
		if (DefaultWeaponClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = this;

			ABaseWeapon* StartingWeapon = GetWorld()->SpawnActor<ABaseWeapon>(DefaultWeaponClass, GetActorLocation(), GetActorRotation(), SpawnParams);
			if (StartingWeapon)
			{
				EquipWeapon(StartingWeapon);
			}
		}
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsLocallyControlled())
	{
		Server_SetLookAtLocation(LookAtTargetSphere->GetComponentLocation());
	}
}


// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APlayerCharacter::StopCrouch);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("TurnCamera", this, &APlayerCharacter::TurnCamera);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::StartFire);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayerCharacter::StartReload);
}

void APlayerCharacter::StartCrouch()
{
	Crouch();
}

void APlayerCharacter::StopCrouch()
{
	UnCrouch();
}

void APlayerCharacter::MoveForward(float Input)
{
	FVector ForwardDirection = GetActorForwardVector();
	AddMovementInput(ForwardDirection, Input);
}

void APlayerCharacter::MoveRight(float Input)
{
	FVector RightDirection = GetActorRightVector();
	AddMovementInput(RightDirection, Input);

}

void APlayerCharacter::TurnCamera(float Input)
{
	AddControllerYawInput(Input);

}

void APlayerCharacter::LookUp(float Input)
{
	AddControllerPitchInput(-Input);
}

void APlayerCharacter::Server_TakeDamage_Implementation(float Damage)
{
	UE_LOG(LogTemp, Warning, TEXT("Server_TakeDamage: Am fost lovit! Damage: %f. CurrentHealth: %f. IsDead() este: %s"),
		Damage,
		CurrentHealth,
		IsDead() ? TEXT("ADEVARAT") : TEXT("FALS"));

	if (Damage <= 0.f || IsDead())
	{return;

	}
	
	TakeDamage(Damage);
	if (IsDead())
	{
		AMyGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AMyGameModeBase>();
		if (GameMode)
		{
			GameMode->PlayerDied(GetController(), this);
			if (Controller)
			{
				Controller->UnPossess();
			}
		}
		Multicast_OnDeath();

		//respawn
		//death animation
		//etc
	}
}

void APlayerCharacter::TakeDamage(float Damage)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
	if (HasAuthority())
	{
		OnRep_HealthChanged();
	}
}

void APlayerCharacter::Multicast_OnDeath_Implementation()
{

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	//GetCharacterMovement()->DisableMovement();

	GetCapsuleComponent()->SetCollisionProfileName(FName("NoCollision"));

	GetMesh()->SetCollisionProfileName(FName("Ragdoll")); 
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetOwnerNoSee(false); 

	FpArms->SetVisibility(false);
}

void APlayerCharacter::Heal(float Amount)
{
	if (Amount <= 0.f || IsDead())return;
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.f, MaxHealth);
	if (HasAuthority())
	{
		OnRep_HealthChanged();
	}
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCharacter, CurrentHealth);
	DOREPLIFETIME(APlayerCharacter, LookAtLocation_Replicated);
	DOREPLIFETIME(APlayerCharacter, CurrentWeapon);
}

void APlayerCharacter::OnRep_HealthChanged()
{
	UE_LOG(LogTemp, Warning, TEXT("%s health updated: %.1f"), *GetName(), CurrentHealth);

	if (CurrentHealth < LastHealth)
	{
		if (IsLocallyControlled() && DamageEffectWidgetClass)
		{
			UUserWidget* Widget = CreateWidget<UUserWidget>(GetController<APlayerController>(), DamageEffectWidgetClass);
			Widget->AddToViewport();
		 }
	}

	if (HealthWidget)
	{
		HealthWidget->UpdateHealth(GetHealthPercent());
	}

	LastHealth = CurrentHealth;
}

bool APlayerCharacter::IsDead() const
{
	return CurrentHealth <= 0.f;
}

float APlayerCharacter::GetHealthPercent() const
{
	return CurrentHealth / MaxHealth;
}

void APlayerCharacter::Server_SetLookAtLocation_Implementation(FVector NewLocation)
{
	LookAtLocation_Replicated = NewLocation;
}

void APlayerCharacter::OnRep_AimRotation()
{

}

void APlayerCharacter::EquipWeapon(ABaseWeapon* WeaponToEquip)
{
	if (HasAuthority())
	{
		if (WeaponToEquip)
		{
			CurrentWeapon = WeaponToEquip;
			CurrentWeapon->SetOwner(this);
			OnRep_CurrentWeapon();
		}
	}
}

void APlayerCharacter::OnRep_CurrentWeapon()
{
	if (CurrentWeapon)
	{
		const FName SocketName = FName("ik_hand_gunPistol");

		if (IsLocallyControlled())
		{
			CurrentWeapon->AttachToComponent(FpArms, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
		}
		else
		{
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
		}
	}
}

void APlayerCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		Server_StartFire();
	}
}

void APlayerCharacter::Server_StartFire_Implementation()
{
	if (CurrentWeapon)
	{
		FVector Loc; FRotator Rot;
		GetActorEyesViewPoint(Loc, Rot);
		CurrentWeapon->Fire(Loc, Rot);
	}
}

void APlayerCharacter::StartReload()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Reload();
	}
}

void APlayerCharacter::Multicast_HandleRespawn_Implementation()
{
	USkeletalMeshComponent* MyMesh = GetMesh();
	if (MyMesh)
	{
		MyMesh->SetSimulatePhysics(false);
		MyMesh->SetCollisionProfileName(TEXT("CharacterMesh")); 
		MyMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		MyMesh->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		MyMesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, -90.f, 0.f));
		 
		MyMesh->SetOwnerNoSee(true);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);

	GetCapsuleComponent()->SetCollisionProfileName(FName("PlayerCharacter"));

	GetCharacterMovement()->Activate();

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	if (FpArms)
	{
		FpArms->SetVisibility(true); 
	}

	ResetHealth();
}

void APlayerCharacter::Client_SaveCameraDefaults()
{
	if (Camera)
	{
		OriginalCameraParent = Camera->GetAttachParent();
		OriginalCameraTransform = Camera->GetRelativeTransform();
		bOriginalUsePawnControlRotation = Camera->bUsePawnControlRotation;
	}
}

void APlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (Controller)
	{
		if (!OriginalCameraParent)
		{
			Client_SaveCameraDefaults();
		}

		if (Camera && OriginalCameraParent)
		{
			Camera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			Camera->AttachToComponent(OriginalCameraParent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Camera->SetRelativeTransform(OriginalCameraTransform);
			Camera->bUsePawnControlRotation = bOriginalUsePawnControlRotation;
		}

		APlayerController* Pc = Cast<APlayerController>(Controller);
		if (Pc)
		{
			Pc->EnableInput(Pc);
		}
	}
}

void APlayerCharacter::UnPossessed()
{
	Super::UnPossessed();


	if (IsDead()) 
	{
		Client_AttachCameraToRagdoll(); 
	}
}

void APlayerCharacter::Client_AttachCameraToRagdoll_Implementation()
{
	if (Camera && GetMesh())
	{
		if (!OriginalCameraParent)
		{
			Client_SaveCameraDefaults();
		}

		Camera->bUsePawnControlRotation = false;
		Camera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		FName BoneToFollow = FName("pelvis");
		Camera->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, BoneToFollow);

		APlayerController* Pc = Cast<APlayerController>(GetController());
		if (Pc)
		{
			Pc->DisableInput(Pc);
		}
	}
}
