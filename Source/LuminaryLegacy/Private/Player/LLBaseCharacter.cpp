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
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"


DEFINE_LOG_CATEGORY_STATIC(BaseCharacterLog, All, All);

// Sets default values
ALLBaseCharacter::ALLBaseCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    TPSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPSpringArmComponent"));
    TPSpringArmComponent->SetupAttachment(GetRootComponent());
    TPSpringArmComponent->bUsePawnControlRotation = true;

    TPCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TPCameraComponent"));
    TPCameraComponent->SetupAttachment(TPSpringArmComponent);
}

// Called when the game starts or when spawned
void ALLBaseCharacter::BeginPlay()
{
    Super::BeginPlay();
  
    if (ALLPlayerController* PlayerController = Cast<ALLPlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(InputMappingContext.LoadSynchronous(), 0);
        }
    }
    
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

    UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);

    Input->BindAction(InputMoving, ETriggerEvent::Triggered, this, &ALLBaseCharacter::Moving);
    Input->BindAction(CameraSwitch, ETriggerEvent::Triggered, this, &ALLBaseCharacter::SwitchCamera);
    Input->BindAction(Look, ETriggerEvent::Triggered, this, &ALLBaseCharacter::Looking);
    Input->BindAction(Jumping, ETriggerEvent::Started, this, &ALLBaseCharacter::Jump);
    Input->BindAction(Jumping, ETriggerEvent::Completed, this, &ALLBaseCharacter::StopJumping);
}

void ALLBaseCharacter::Moving(const FInputActionValue& Value)
{
    FVector2d Direction = Value.Get<FVector2d>();
    if (!SideWalk)
    {
        FVector RightDirection = UKismetMathLibrary::GetRightVector(FRotator(0.0f, GetControlRotation().Yaw, GetControlRotation().Roll));
        FVector ForwardDirection = UKismetMathLibrary::GetForwardVector(FRotator(0.0f, GetControlRotation().Yaw, 0.0f));
        AddMovementInput(ForwardDirection, Direction.Y);
        AddMovementInput(RightDirection, Direction.X);
    }
    else
    {
        AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Direction.X);
        float ZRotation = Direction.X < 0.0f ? 180.0f : 0.0f;
        UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetControlRotation(FRotator(0.0f, ZRotation, 0.0f));
    }
}

void ALLBaseCharacter::Looking(const FInputActionValue& Value)
{
    if (!SideWalk)
    {
        FVector2d Direction = Value.Get<FVector2d>();
        AddControllerPitchInput(Direction.Y);
        AddControllerYawInput(-Direction.X);
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
                GetCharacterMovement()->bOrientRotationToMovement = false;
                GetCharacterMovement()->bUseControllerDesiredRotation = true;
                SetupCamera(View);
            }
        break;
        case EViewType::ThirdPerson:
            if (PlayerController)
            {
                SideWalk = false;
                PlayerController->SetViewTargetWithBlend(this, BlendTime);
                GetCharacterMovement()->bOrientRotationToMovement = true;
                GetCharacterMovement()->bUseControllerDesiredRotation = false;
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
            GetCharacterMovement()->bUseControllerDesiredRotation = true;
            break;
        case EViewType::ThirdPerson:
            bUseControllerRotationYaw = false;
            bUseControllerRotationPitch = false;
            GetCharacterMovement()->bOrientRotationToMovement = true;
            break;
    }
}

ALLPlayerCamera* ALLBaseCharacter::CreateCamera()
{
    return GetWorld()->SpawnActor<ALLPlayerCamera>(PlayerCamera2D, this->GetActorTransform());
}
