// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LLPlayerCamera.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class LUMINARYLEGACY_API ALLPlayerCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALLPlayerCamera();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
    ACharacter* CameraTarget;

    UFUNCTION()
    void OnPlayerLanded(const FHitResult& Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
    USceneComponent* SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
    UCameraComponent* CameraComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
    float CameraHightTarget = 0.0f;
    float ZHightThreshold = 200.0f;
    float ZHightUpperThreshold = 350.0f;
};
