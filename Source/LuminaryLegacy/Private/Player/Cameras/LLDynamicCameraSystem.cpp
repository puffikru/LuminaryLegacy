// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Cameras/LLDynamicCameraSystem.h"

#include "LLBaseCharacter.h"
#include "LLPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ALLDynamicCameraSystem::ALLDynamicCameraSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    DefaultRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
    SetRootComponent(DefaultRootComponent);

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    SpringArmComponent->SetupAttachment(GetRootComponent());

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent);

    StartCamera = CreateDefaultSubobject<UBoxComponent>(TEXT("StartCamera"));
    StartCamera->SetupAttachment(GetRootComponent());

    EndCamera = CreateDefaultSubobject<UBoxComponent>(TEXT("EndCamera"));
    EndCamera->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ALLDynamicCameraSystem::BeginPlay()
{
	Super::BeginPlay();

    CameraTarget = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    StartCamera->OnComponentBeginOverlap.AddDynamic(this, &ALLDynamicCameraSystem::OnStart);
    EndCamera->OnComponentBeginOverlap.AddDynamic(this, &ALLDynamicCameraSystem::OnEnd);
}

// Called every frame
void ALLDynamicCameraSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    ALLPlayerController* PlayerController = Cast<ALLPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (PlayerController && PlayerController->GetViewTarget() == this)
    {
        if (bIsStationary)
        {
            CameraComponent->SetWorldRotation(TrackPlayerRotation());
        }
        else
        {
            SpringArmComponent->SetWorldLocation(TrackPlayerMovement(DeltaTime));
        }
    }
}

void ALLDynamicCameraSystem::OnStart(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    if (OtherActor->IsA(ALLBaseCharacter::StaticClass()))
    {
        ALLPlayerController* PlayerController = Cast<ALLPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
        if (!PlayerController) return;
        PlayerController->SetViewTargetWithBlend(this, BlendTime);
        if (!bHasOffset)
        {
            Offset = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation() - SpringArmComponent->GetComponentLocation();
            bHasOffset = true;
        }
    }
}

void ALLDynamicCameraSystem::OnEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor->IsA(ALLBaseCharacter::StaticClass()))
    {
        ALLBaseCharacter* Player = Cast<ALLBaseCharacter>(OtherActor);
        ALLPlayerController* PlayerController = Cast<ALLPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
        if (!PlayerController) return;
        PlayerController->SetViewTargetWithBlend(Player, BlendTime);
    }
}

FRotator ALLDynamicCameraSystem::TrackPlayerRotation() const
{
    return UKismetMathLibrary::FindLookAtRotation(CameraComponent->GetComponentLocation(), UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation());
}

FVector ALLDynamicCameraSystem::TrackPlayerMovement(float DeltaTime) const
{
    FVector CameraLocation = GetActorLocation() - Offset;
    FVector PlayerLocation = Cast<ALLBaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->GetActorLocation();
    FVector Diff = FVector(1.0f, 1.0f, 1.0f) - TrackPlayer;
    FVector Target = PlayerLocation * TrackPlayer + CameraLocation * Diff;
    return FMath::VInterpTo(CameraLocation, Target, DeltaTime, 0.0f);
}

FVector ALLDynamicCameraSystem::GetCharacterOffset() const
{
    if (!CameraTarget) return FVector::ZeroVector;

    return CameraTarget->GetActorLocation() + CameraTarget->GetActorForwardVector() * CameraOffset * -1.0f + CameraHight;
}

