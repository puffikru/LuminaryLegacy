// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Cameras/LLCameraTrigger.h"
#include "LLBaseCharacter.h"
#include "Components/BoxComponent.h"
#include "LLCoreTypes.h"

// Sets default values
ALLCameraTrigger::ALLCameraTrigger()
{
    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    SetRootComponent(BoxComponent);
}

// Called when the game starts or when spawned
void ALLCameraTrigger::BeginPlay()
{
	Super::BeginPlay();

    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ALLCameraTrigger::OnBeginOverlapped);
}

void ALLCameraTrigger::OnBeginOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ALLBaseCharacter* PlayerCharacter = Cast<ALLBaseCharacter>(OtherActor);
    PlayerCharacter->SetCameraType(CameraType);
}

