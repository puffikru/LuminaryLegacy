// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LLCoreTypes.h"
#include "GameFramework/Actor.h"
#include "LLCameraTrigger.generated.h"

class UBoxComponent;

UCLASS()
class LUMINARYLEGACY_API ALLCameraTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALLCameraTrigger();

    UPROPERTY(EditAnywhere, Category="Camera")
    EViewType CameraType = EViewType::SideView;
    
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category="Components")
    UBoxComponent* BoxComponent;

private:
    UFUNCTION()
    void OnBeginOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
