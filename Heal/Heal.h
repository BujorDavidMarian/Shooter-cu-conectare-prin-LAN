// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Heal.generated.h"

UCLASS()
class INVATARECPP_API AHeal : public AActor
{
	GENERATED_BODY()
	
public:	
	AHeal();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
	UBoxComponent* CollisionBox;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Heal")
	float HealAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heal")
	float RespawnTime = 10.0f;

	FTimerHandle RespawnTimerHandle;

	void RespawnPickup();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DisablePickup();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EnablePickup();


public:	
	

};
