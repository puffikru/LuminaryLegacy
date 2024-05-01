// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LLSplineCamera.generated.h"

class UBoxComponent;
class USplineComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class LUMINARYLEGACY_API ALLSplineCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALLSplineCamera();

    UPROPERTY(EditAnywhere, Category="Camera")
    float CameraBlendTime = 1.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category="Components")
    USceneComponent* SceneRootComponent;

    UPROPERTY(EditDefaultsOnly, Category="Components")
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(EditDefaultsOnly, Category="Components")
    UCameraComponent* CameraComponent;

    UPROPERTY(EditDefaultsOnly, Category="Components")
    USplineComponent* Spline;

    UPROPERTY(EditDefaultsOnly, Category="Components")
    UBoxComponent* BoxComponent;
    
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
    UFUNCTION()
    void StartFollow(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
