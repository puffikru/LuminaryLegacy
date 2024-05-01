// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LLDynamicCameraSystem.generated.h"

class UBoxComponent;
class UCameraComponent;
class USpringArmComponent;
class USceneComponent;

UCLASS()
class LUMINARYLEGACY_API ALLDynamicCameraSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALLDynamicCameraSystem();

    UPROPERTY(EditAnywhere, Category="Camera")
    float BlendTime = 0.0f;

    UPROPERTY(EditAnywhere, Category="Camera")
    bool bIsStationary = false;

    UPROPERTY(EditAnywhere, Category="Camera")
    float CameraOffset = 600.0f;

    UPROPERTY(EditAnywhere, Category="Camera")
    FVector CameraHight = FVector(0.0f, 0.0f, 300.0f);

    ACharacter* CameraTarget;

    UPROPERTY(EditAnywhere, Category="Camera")
    FVector TrackPlayer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category="Components")
    USceneComponent* DefaultRootComponent;

    UPROPERTY(EditAnywhere, Category="Components")
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(EditAnywhere, Category="Components")
    UCameraComponent* CameraComponent;

    UPROPERTY(EditAnywhere, Category="Components")
    UBoxComponent* StartCamera;

    UPROPERTY(EditAnywhere, Category="Components")
    UBoxComponent* EndCamera;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
    bool bHasOffset = false;
    FVector Offset;
    
    UFUNCTION()
    void OnStart(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

    UFUNCTION()
    void OnEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

    FRotator TrackPlayerRotation() const;
    FVector TrackPlayerMovement(float DeltaTime) const;

    FVector GetCharacterOffset() const;
    
};
