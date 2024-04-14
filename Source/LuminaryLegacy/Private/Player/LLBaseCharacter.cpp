// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/LLBaseCharacter.h"

#include "LLPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "LLCoreTypes.h"
#include "Cameras/LLPlayerCamera.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Camera/CameraActor.h"
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
    TPSpringArmComponent->TargetOffset.Z = 150.0f;
    TPSpringArmComponent->TargetArmLength = 250.0f;

    TPCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TPCameraComponent"));
    TPCameraComponent->SetupAttachment(TPSpringArmComponent);
    TPCameraComponent->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));

    TDSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("TopDownSpringArm"));
    TDSpringArmComponent->SetupAttachment(GetRootComponent());
    TDSpringArmComponent->bUsePawnControlRotation = true;

    TDCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
    TDCameraComponent->SetupAttachment(TDSpringArmComponent);

    TPShoulderSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("ShoulderSpringArm"));
    TPShoulderSpringArmComponent->SetupAttachment(GetRootComponent());
    TPShoulderSpringArmComponent->bUsePawnControlRotation = true;
    TPShoulderSpringArmComponent->TargetArmLength = 200.0f;
    TPShoulderSpringArmComponent->SocketOffset = FVector(0.0f, 50.0f, 100.0f);

    TPShoulderCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ShoulderCamera"));
    TPShoulderCameraComponent->SetupAttachment(TPShoulderSpringArmComponent);
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

    CurrentTPCameraViewType = ETPCameraType::None;
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
    Input->BindAction(CameraSwitch, ETriggerEvent::Triggered, this, &ALLBaseCharacter::SwitchCameraType);
    Input->BindAction(Look, ETriggerEvent::Triggered, this, &ALLBaseCharacter::Looking);
    Input->BindAction(Jumping, ETriggerEvent::Started, this, &ALLBaseCharacter::Jump);
    Input->BindAction(Jumping, ETriggerEvent::Completed, this, &ALLBaseCharacter::StopJumping);
    Input->BindAction(TPCameraDefault, ETriggerEvent::Triggered, this, &ALLBaseCharacter::SwitchToCameraDefault);
    Input->BindAction(TPCameraShoulder, ETriggerEvent::Triggered, this, &ALLBaseCharacter::SwitchToCameraShoulder);
    Input->BindAction(TPCameraTopDown, ETriggerEvent::Triggered, this, &ALLBaseCharacter::SwitchToCameraTopDown);
}

void ALLBaseCharacter::Moving(const FInputActionValue& Value)
{
    const FVector2d Direction = Value.Get<FVector2d>();
    if (!SideWalk)
    {
        // const FVector RightDirection = UKismetMathLibrary::GetRightVector(FRotator(0.0f, GetControlRotation().Yaw, GetControlRotation().Roll));
        // const FVector ForwardDirection = UKismetMathLibrary::GetForwardVector(FRotator(0.0f, GetControlRotation().Yaw, 0.0f));
        float Rot = Cast<ALLPlayerController>(GetController())->PlayerCameraManager->GetCameraRotation().Yaw;
        FRotator temp = FRotator(0.0f, Rot, 0.0f);
        FVector Right = UKismetMathLibrary::GetRightVector(temp);
        FVector Forw = UKismetMathLibrary::GetForwardVector(temp);
        AddMovementInput(Forw, Direction.Y);
        AddMovementInput(Right, Direction.X);
        // AddMovementInput(ForwardDirection, Direction.Y);
        // AddMovementInput(RightDirection, Direction.X);
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

void ALLBaseCharacter::SwitchCameraType()
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

void ALLBaseCharacter::SwitchToCameraShoulder()
{
    SetCameraView(ETPCameraType::Shoulder);
}

void ALLBaseCharacter::SwitchToCameraTopDown()
{
    SetCameraView(ETPCameraType::TopDown);
}

void ALLBaseCharacter::SwitchToCameraDefault()
{
    SetCameraView(ETPCameraType::Default);
}

void ALLBaseCharacter::SetView(EViewType View, float BlendTime)
{
    ALLPlayerController* PlayerController = Cast<ALLPlayerController>(GetController());
    if (!PlayerController) return;
    
    switch (View)
    {
        case EViewType::SideView:
            SideWalk = true;
            if (!SideCamera)
            {
                SideCamera = CreateCamera();
            }
            CurrentTPCameraViewType = ETPCameraType::None;
            SideCamera->SetActorRotation(FRotator(0.0f, -90.0f, 0.0f));
            PlayerController->SetViewTargetWithBlend(SideCamera, BlendTime);
            GetCharacterMovement()->bOrientRotationToMovement = false;
            GetCharacterMovement()->bUseControllerDesiredRotation = true;
            SetupCamera(View);
        break;
        case EViewType::ThirdPerson:
            SideWalk = false;
            GetCharacterMovement()->bOrientRotationToMovement = true;
            GetCharacterMovement()->bUseControllerDesiredRotation = false;
            SetCameraView(ETPCameraType::Default, BlendTime);
            SetupCamera(View);
        break;
    }
}

void ALLBaseCharacter::SetCameraView(ETPCameraType CameraView, float BlendTime)
{
    if (CurrentTPCameraViewType != ETPCameraType::None && CurrentTPCameraViewType == CameraView) return;
    
    ALLPlayerController* PlayerController = Cast<ALLPlayerController>(GetController());
    if (!PlayerController) return;
    
    switch (CameraView)
    {
        case ETPCameraType::Default:
            CurrentTPCameraViewType = ETPCameraType::Default;
            CurrentCameraComponent = TPCameraComponent;
            TPSpringArmComponent->TargetOffset.Z = 150.0f;
            TPSpringArmComponent->TargetArmLength = 250.0f;
            TPCameraComponent->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));
            break;
        case ETPCameraType::Shoulder:
            CurrentTPCameraViewType = ETPCameraType::Shoulder;
            CurrentCameraComponent = TPShoulderCameraComponent;
            TPShoulderSpringArmComponent->TargetArmLength = 200.0f;
            TPShoulderSpringArmComponent->SocketOffset = FVector(0.0f, 50.0f, 100.0f);
            break;
        case ETPCameraType::TopDown:
            break;
    }

    ACameraActor* TempCamera = CreateCameraFromComponent(CurrentCameraComponent);
    PlayerController->SetViewTargetWithBlend(TempCamera, BlendTime);

    GetWorldTimerManager().SetTimer(CameraBlendTimerHandle, this, &ALLBaseCharacter::ResetCameras, BlendTime);
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

void ALLBaseCharacter::ResetCameras()
{
    switch (CurrentTPCameraViewType)
    {
        case ETPCameraType::Default:
            TPCameraComponent->SetActive(true);
            TPShoulderCameraComponent->SetActive(false);
            TDCameraComponent->SetActive(false);
            break;
        case ETPCameraType::Shoulder:
            TPCameraComponent->SetActive(false);
            TPShoulderCameraComponent->SetActive(true);
            TDCameraComponent->SetActive(false);
            break;
    }

    ALLPlayerController* PlayerController = Cast<ALLPlayerController>(GetController());
    if (!PlayerController) return;

    PlayerController->SetViewTargetWithBlend(this, 0.0f);

    TArray<AActor*> AttachedActors;
    GetAttachedActors(AttachedActors);

    if (AttachedActors.Num())
    {
        for (const auto Actor : AttachedActors)
        {
            if (ACameraActor* Camera = Cast<ACameraActor>(Actor))
            {
                Camera->Destroy();
            }
        }
    }
}

ALLPlayerCamera* ALLBaseCharacter::CreateCamera() const
{
    return GetWorld()->SpawnActor<ALLPlayerCamera>(PlayerCamera2D, this->GetActorTransform());
}

ACameraActor* ALLBaseCharacter::CreateCameraFromComponent(UCameraComponent* CameraComponent) const
{
    ACameraActor* Camera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), CameraComponent->GetComponentTransform());
    Camera->AttachToComponent(CameraComponent, FAttachmentTransformRules::KeepWorldTransform);
    Camera->GetCameraComponent()->bConstrainAspectRatio = false;
    return Camera;
}


