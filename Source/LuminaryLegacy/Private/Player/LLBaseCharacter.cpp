// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/LLBaseCharacter.h"

#include "LLPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "LLCoreTypes.h"
#include "LLPlayerCamera.h"

DEFINE_LOG_CATEGORY_STATIC(BaseCharacterLog, All, All);

// Sets default values
ALLBaseCharacter::ALLBaseCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    TPSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPSpringArmComponent"));
    TPSpringArmComponent->SetupAttachment(GetRootComponent());
    TPSpringArmComponent->bUsePawnControlRotation = true;
    GetCharacterMovement()->bOrientRotationToMovement = true;

    TPCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TPCameraComponent"));
    TPCameraComponent->SetupAttachment(TPSpringArmComponent);
}

// Called when the game starts or when spawned
void ALLBaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->JumpZVelocity = JumpZVelocity;
    GetCharacterMovement()->AirControl = AirControl;
    GetCharacterMovement()->GravityScale = GravityScale;

    SetView(EViewType::SideView);
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
    PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &ALLBaseCharacter::SwitchCamera);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ALLBaseCharacter::Jump);
}

void ALLBaseCharacter::MoveForward(float Value)
{
    if (!SideWalk)
    {
        FVector Direction = UKismetMathLibrary::GetForwardVector(FRotator(0.0f, GetControlRotation().Yaw, 0.0f));
        AddMovementInput(Direction, Value);
    }
}

void ALLBaseCharacter::MoveRight(float Value)
{
    if (!SideWalk)
    {
        FVector Direction = UKismetMathLibrary::GetRightVector(FRotator(0.0f, GetControlRotation().Yaw, GetControlRotation().Roll));
        AddMovementInput(Direction, Value);
    }
    else
    {
        AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
    }
}

void ALLBaseCharacter::SwitchCamera()
{
    if (SideWalk)
    {
        SetView(EViewType::ThirdPerson, 1.0f);
    }
    else
    {
        SetView(EViewType::SideView, 1.0f);
    }
}

void ALLBaseCharacter::SetView(EViewType View, float BlendTime)
{
    ALLPlayerController* PlayerController = Cast<ALLPlayerController>(GetController());
    switch (View)
    {
        case EViewType::SideView:
            if (PlayerController)
            {
                SideWalk = true;
                if (!Camera)
                {
                    Camera = CreateCamera();
                }
                Camera->SetActorRotation(FRotator(0.0f, -90.0f, 0.0f));
                PlayerController->SetViewTargetWithBlend(Camera, BlendTime);
                SetupCamera(View);
            }
        break;
        case EViewType::ThirdPerson:
            if (PlayerController)
            {
                SideWalk = false;
                PlayerController->SetViewTargetWithBlend(this, BlendTime);
                SetupCamera(View);
            }
        break;
    }
}

void ALLBaseCharacter::SetupCamera(EViewType View)
{
    switch(View)
    {
        case EViewType::SideView:
            // GetCharacterMovement()->bOrientRotationToMovement = false;
            // GetCharacterMovement()->bUseControllerDesiredRotation = true;
                GetCharacterMovement()->bOrientRotationToMovement = true;
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("SideView"));
            break;
        case EViewType::ThirdPerson:
            bUseControllerRotationYaw = false;
            bUseControllerRotationPitch = false;
            GetCharacterMovement()->bOrientRotationToMovement = true;
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("ThirdPersonView"));
            break;
    }
}

ALLPlayerCamera* ALLBaseCharacter::CreateCamera()
{
    return GetWorld()->SpawnActor<ALLPlayerCamera>(PlayerCamera2D, this->GetActorTransform());
}
