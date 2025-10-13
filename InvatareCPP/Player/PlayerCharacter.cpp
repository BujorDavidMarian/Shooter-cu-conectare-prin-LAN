// Fill out your copyright notice in the Description page of Project Settings.


#include "InvatareCPP/Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Camera

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("PLayer Camera"));
	Camera->SetupAttachment(RootComponent);
	Camera->bUsePawnControlRotation = true;
	
	GetMesh()->bOwnerNoSee = true;
	FpArms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Arms"));
	FpArms->SetupAttachment(Camera);
	FpArms->bCastDynamicShadow = false;
	FpArms->CastShadow = false;
	FpArms->SetOnlyOwnerSee(true);

	bReplicates = true;

	

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateStamina();

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("TurnCamera", this, &APlayerCharacter::TurnCamera);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::EndSprint);


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

void APlayerCharacter::StartSprint()
{
	if (bHasStamina)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;

		if (GetVelocity().Size() >= 0.5)
		{
			bIsRunning = true;
		}
		else
		{
			bIsRunning = false;
		}
	}
}

void APlayerCharacter::EndSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	bIsRunning = false;
}

void APlayerCharacter::UpdateStamina()
{
	//Drain Stamina
	if (bIsRunning)
	{
		CurrentStamina -= StaminaDrainTime;
		CurrentRefilDelayTime = DelayBeforeRefil;
	}

	if (!bIsRunning && CurrentStamina < MaxStamina)
	{
		CurrentRefilDelayTime--;
		if (CurrentRefilDelayTime <= 0)
		{
			CurrentStamina += StaminaRefilTime;
		}
	}

	if (CurrentStamina <= 0)
	{
		bHasStamina = false;
		EndSprint();
	}
	else if(CurrentStamina == MaxStamina)
	{
		bHasStamina = true;
	}
}

