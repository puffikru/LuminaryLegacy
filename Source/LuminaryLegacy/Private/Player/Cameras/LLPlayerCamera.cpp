// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Cameras/LLPlayerCamera.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "LLBaseCharacter.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(PlayerCameraLog, All, All);

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
    SpringArmComponent->SocketOffset.Z = SprintArmComponentSocketOffsetZ;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent);
}

void ALLPlayerCamera::OnPlayerLanded(const FHitResult& Hit)
{
    const float HightDiff = FMath::Abs(Hit.Location.Z - CameraHightTarget);
    if (HightDiff > ZHightThreshold)
    {
        CameraHightTarget = Hit.Location.Z;
    }
}

// Called when the game starts or when spawned
void ALLPlayerCamera::BeginPlay()
{
	Super::BeginPlay();

    CameraTarget = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    if (CameraTarget)
    {
        CameraTarget->LandedDelegate.AddDynamic(this, &ALLPlayerCamera::OnPlayerLanded);
        CameraHightTarget = CameraTarget->GetActorLocation().Z;
        SetActorLocation(FVector(CameraTarget->GetActorLocation().X + CameraOffset, CameraTarget->GetActorLocation().Y, CameraTarget->GetActorLocation().Z));
    }
    else
    {
        UE_LOG(PlayerCameraLog, Error, TEXT("No CameraTarget was set."));
    }
}

// Called every frame
void ALLPlayerCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    CheckTracking();
    
    if (bIsCameraTracking)
    {
        const FVector NewLocation = FMath::VInterpTo(GetActorLocation(), GetCharacterOffset(), DeltaTime, InterpSpeed);
        SetActorLocation(FVector(NewLocation.X, NewLocation.Y, GetZHeight()));
    }
}

float ALLPlayerCamera::GetZHeight() const
{
    if (CameraTarget)
    {
        const float CameraHightDifferences = FMath::Abs(GetActorLocation().Z - CameraTarget->GetActorLocation().Z);
        const float NewHeight = CameraHightDifferences > ZHightUpperThreshold ? CameraTarget->GetActorLocation().Z : CameraHightTarget;
        return FMath::FInterpTo(GetActorLocation().Z, NewHeight, GetWorld()->GetDeltaSeconds(), InterpSpeed);
    }
    return 0.0f;
}

FVector ALLPlayerCamera::GetCharacterOffset() const
{
    if (!CameraTarget)
    {
        UE_LOG(PlayerCameraLog, Error, TEXT("No CameraTarget was set."));
        return FVector::ZeroVector;
    }
    const float Dot = FMath::Sign(FVector::DotProduct(CameraTarget->GetActorForwardVector(), GetActorRightVector()));
    return CameraTarget->GetActorLocation() + GetActorRightVector() * Dot * CameraOffset;
}

void ALLPlayerCamera::CheckTracking()
{
    const float Distance = FVector::Dist(GetActorLocation(), GetCharacterOffset());

    if (CheckCameraDirection())
    {
        // InterpSpeed = 2.0f;
        if (Distance > ForwardDirectionDistanceMaxThreshold)
        {
            bIsCameraTracking = true;
        }
        else if (Distance < ForwardDirectionDistanceMinThreshold)
        {
            bIsCameraTracking = false;
        }
    }
    else
    {
        // InterpSpeed = 2.0f;
        if (Distance > BackDirectionDistanceMaxThreshold)
        {
            bIsCameraTracking = true;
        }
    }
}

bool ALLPlayerCamera::CheckCameraDirection() const
{
    if (!CameraTarget) return false;
    
    const FVector ToCameraDirection = GetActorLocation() - CameraTarget->GetActorLocation();
    return FMath::Sign(FVector::DotProduct(ToCameraDirection, CameraTarget->GetActorForwardVector())) > 0.0f ? true : false;
}

