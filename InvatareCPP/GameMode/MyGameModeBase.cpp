// Fill out your copyright notice in the Description page of Project Settings.


#include "InvatareCPP/GameMode/MyGameModeBase.h"
#include "MyGameModeBase.h"
#include "InvatareCPP/Player/PlayerCharacter.h"

AMyGameModeBase::AMyGameModeBase()
{
	DefaultPawnClass = APlayerCharacter::StaticClass(); //clasa PlayerCharacter
	PlayerControllerClass = APlayerController::StaticClass();
}
