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
#include "Components/CapsuleComponent.h"


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
  
    if (const ALLPlayerController* PlayerController = Cast<ALLPlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(InputMappingContext.LoadSynchronous(), 0);
        }
    }

    check(GetCharacterMovement());
    
    GetCharacterMovement()->JumpZVelocity = JumpZVelocity;
    GetCharacterMovement()->AirControl = AirControl;
    GetCharacterMovement()->GravityScale = GravityScale;
    GetCharacterMovement()->RotationRate.Yaw = RotationRateYaw;

    SetView(EViewType::SideView);
}

// Called every frame
void ALLBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (SideWalk)
    {
        JumpThrough();
    }
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
    const FVector2d Direction = Value.Get<FVector2d>();
    if (!SideWalk)
    {
        const FVector RightDirection = UKismetMathLibrary::GetRightVector(FRotator(0.0f, GetControlRotation().Yaw, GetControlRotation().Roll));
        const FVector ForwardDirection = UKismetMathLibrary::GetForwardVector(FRotator(0.0f, GetControlRotation().Yaw, 0.0f));
        AddMovementInput(ForwardDirection, Direction.Y);
        AddMovementInput(RightDirection, Direction.X);
    }
    else
    {
        AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Direction.X);
        const float ZRotation = Direction.X < 0.0f ? 180.0f : 0.0f;
        UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetControlRotation(FRotator(0.0f, ZRotation, 0.0f));
    }
}

void ALLBaseCharacter::Looking(const FInputActionValue& Value)
{
    if (!SideWalk)
    {
        const FVector2d Direction = Value.Get<FVector2d>();
        AddControllerPitchInput(Direction.Y);
        AddControllerYawInput(-Direction.X);
    }
}

void ALLBaseCharacter::JumpThrough() const
{
    const float dot = FMath::Sign(FVector::DotProduct(GetVelocity(), GetActorUpVector()));
    FHitResult Hit;
    const FVector Start = GetActorLocation();
    const FVector End = FVector(Start.X, Start.Y, Start.Z + dot * 400.0f);
    GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_GameTraceChannel1);
    GetComponentByClass<UCapsuleComponent>()->IgnoreComponentWhenMoving(Hit.GetComponent(), dot > 0.0f);
}

void ALLBaseCharacter::SwitchCamera()
{
    if (SideWalk)
    {
        SetView(EViewType::ThirdPerson, CameraBlendTime);
    }
    else
    {
        SetView(EViewType::SideView, CameraBlendTime);
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

ALLPlayerCamera* ALLBaseCharacter::CreateCamera() const
{
    return GetWorld()->SpawnActor<ALLPlayerCamera>(PlayerCamera2D, this->GetActorTransform());
}
