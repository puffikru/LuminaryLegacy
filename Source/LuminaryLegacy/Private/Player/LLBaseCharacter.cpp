// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LLBaseCharacter.h"

// Sets default values
ALLBaseCharacter::ALLBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALLBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALLBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALLBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

