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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
    float CameraOffset = 350.0f;
    
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
    float CameraHightTarget;
    float ZHightThreshold = 200.0f;
    float ZHightUpperThreshold = 300.0f;
    bool bIsCameraTracking = false;
    float InterpSpeed = 3.0f;

    float GetZHeight() const;
    FVector GetCharacterOffset() const;
    void CheckTracking();
    bool CheckCameraDirection() const;

};
