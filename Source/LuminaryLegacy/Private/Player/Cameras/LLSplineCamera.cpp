// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Cameras/LLSplineCamera.h"

#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "LLPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LLBaseCharacter.h"

// Sets default values
ALLSplineCamera::ALLSplineCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
    SetRootComponent(SceneRootComponent);

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    SpringArmComponent->SetupAttachment(GetRootComponent());

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent);

    Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
    Spline->SetupAttachment(GetRootComponent());

    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    BoxComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ALLSplineCamera::BeginPlay()
{
	Super::BeginPlay();

    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ALLSplineCamera::StartFollow);
}

// Called every frame
void ALLSplineCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    ALLBaseCharacter* PlayerCharacter = Cast<ALLBaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (PlayerCharacter)
    {
        FVector NewLocation = Spline->FindLocationClosestToWorldLocation(PlayerCharacter->GetActorLocation(), ESplineCoordinateSpace::World);
        SpringArmComponent->SetWorldLocation(NewLocation);

        FVector NewDirection = Spline->FindDirectionClosestToWorldLocation(PlayerCharacter->GetActorLocation(), ESplineCoordinateSpace::World);
        // FRotator NewRotation = UKismetMathLibrary::MakeRotFromX(NewDirection);
        // SpringArmComponent->SetWorldRotation(FRotator(-30.0f, NewRotation.Yaw, 0.0f));
        SpringArmComponent->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(CameraComponent->GetComponentLocation(), UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation()));
    }
}

void ALLSplineCamera::StartFollow(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ALLPlayerController* PlayerController = Cast<ALLPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (PlayerController)
    {
        PlayerController->SetViewTargetWithBlend(this, CameraBlendTime);
    }
}

