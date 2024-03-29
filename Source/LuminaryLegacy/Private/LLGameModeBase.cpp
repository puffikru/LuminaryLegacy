// Fill out your copyright notice in the Description page of Project Settings.


#include "LLGameModeBase.h"
#include "Player/LLBaseCharacter.h"
#include "Player/LLPlayerController.h"

ALLGameModeBase::ALLGameModeBase()
{
    DefaultPawnClass = ALLBaseCharacter::StaticClass();
    PlayerControllerClass = ALLPlayerController::StaticClass();
}
