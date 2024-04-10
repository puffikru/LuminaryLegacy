// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LLPlayerCamera.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "LLBaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


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

void ALLPlayerCamera::OnPlayerLanded(const FHitResult& Hit)
{
    float HightDiff = FMath::Abs(Hit.Location.Z - CameraHightTarget);
    if (HightDiff > ZHightThreshold)
    {
        CameraHightTarget = Hit.Location.Z;
    }
    GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Green, TEXT("Landed"));
}

// Called when the game starts or when spawned
void ALLPlayerCamera::BeginPlay()
{
	Super::BeginPlay();

    CameraTarget = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    CameraTarget->LandedDelegate.AddDynamic(this, &ALLPlayerCamera::OnPlayerLanded);
}

// Called every frame
void ALLPlayerCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    CheckTracking();
    
    if (CameraTarget)
    {
        if (bIsCameraTracking)
        {
            FVector NewLocation = FMath::VInterpTo(GetActorLocation(), GetCharacterOffset(), DeltaTime, 3.0f);
            SetActorLocation(FVector(NewLocation.X, NewLocation.Y, GetZHeight()));
        }
    }
}

float ALLPlayerCamera::GetZHeight() const
{
    float CameraHightDifferences = FMath::Abs(GetActorLocation().Z - CameraTarget->GetActorLocation().Z);
    float NewHeight = CameraHightDifferences > ZHightUpperThreshold ? CameraTarget->GetActorLocation().Z : CameraHightTarget;
    return FMath::FInterpTo(GetActorLocation().Z, NewHeight, GetWorld()->GetDeltaSeconds(), 3.0f);
}

FVector ALLPlayerCamera::GetCharacterOffset() const
{
    if (!CameraTarget) return FVector::ZeroVector;
    float dot = FMath::Sign(FVector::DotProduct(CameraTarget->GetActorForwardVector(), GetActorRightVector()));
    return CameraTarget->GetActorLocation() + GetActorRightVector() * dot * CameraOffset;
}

void ALLPlayerCamera::CheckTracking()
{
    float Distnace = FVector::Dist(GetActorLocation(), GetCharacterOffset());

    if (!bIsCameraTracking)
    {
        if (GetZHeight() != GetActorLocation().Z || CameraTarget && CameraTarget->GetVelocity().Length() > 300.0f && Distnace > 650.0f)
        {
            bIsCameraTracking = true;
        }
    }
    else
    {
        if (Distnace < 150.0f)
        {
            bIsCameraTracking = false;
        }
    }
}

