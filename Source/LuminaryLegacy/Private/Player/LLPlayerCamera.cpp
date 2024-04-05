// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LLPlayerCamera.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "LLPlayerController.h"
#include "LLBaseCharacter.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ALLPlayerCamera::ALLPlayerCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SceneRoot->SetupAttachment(GetRootComponent());
    
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    SpringArmComponent->SetupAttachment(SceneRoot);
    SpringArmComponent->bDoCollisionTest = false;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent);
}

// Called when the game starts or when spawned
void ALLPlayerCamera::BeginPlay()
{
	Super::BeginPlay();

    CameraTarget = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

// Called every frame
void ALLPlayerCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (CameraTarget)
    {
        FVector NewLocation = FMath::VInterpTo(GetActorLocation(), CameraTarget->GetActorLocation(), DeltaTime, 3.0f);
        SetActorLocation(NewLocation);
    }
}

