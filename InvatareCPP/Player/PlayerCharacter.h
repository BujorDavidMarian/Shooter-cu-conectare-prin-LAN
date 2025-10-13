
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class INVATARECPP_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	UPROPERTY(EditAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
	class UMeshComponent* FpArms;

	void MoveForward(float Input);

	void MoveRight(float Input);

	void TurnCamera(float Input);

	void LookUp(float Input);


	//Sprinting
	void StartSprint();
	void EndSprint();

	UPROPERTY(EditAnywhere, Category = "Movement")
	float WalkSpeed;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float RunSpeed;

	bool bIsRunning;

	//Stamina

	void UpdateStamina();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float CurrentStamina;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float StaminaDrainTime;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float StaminaRefilTime;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float DelayBeforeRefil;

	float CurrentRefilDelayTime;
	bool bHasStamina;
};
