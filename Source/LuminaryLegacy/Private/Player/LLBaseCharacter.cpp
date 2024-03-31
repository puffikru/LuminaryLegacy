// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LLBaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(BaseCharacterLog, All, All);

// Sets default values
ALLBaseCharacter::ALLBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    SpringArmComponent->SetupAttachment(GetRootComponent());
    SpringArmComponent->bUsePawnControlRotation = true;
    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    
    GetCharacterMovement()->bOrientRotationToMovement = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent);
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

    PlayerInputComponent->BindAxis("MoveForward", this, &ALLBaseCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ALLBaseCharacter::MoveRight);
    PlayerInputComponent->BindAxis("LookUp", this, &ALLBaseCharacter::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("TurnAround", this, &ALLBaseCharacter::AddControllerYawInput);
}

void ALLBaseCharacter::MoveForward(float Value)
{
    FVector Direction = UKismetMathLibrary::GetForwardVector(FRotator(0.0f, GetControlRotation().Yaw, 0.0f));
    AddMovementInput(Direction, Value);
}

void ALLBaseCharacter::MoveRight(float Value)
{
    FVector Direction = UKismetMathLibrary::GetRightVector(FRotator(0.0f, GetControlRotation().Yaw, GetControlRotation().Roll));
    AddMovementInput(Direction, Value);
}

