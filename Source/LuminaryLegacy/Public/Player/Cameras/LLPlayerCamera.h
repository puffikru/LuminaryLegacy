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

    UPROPERTY(EditAnywhere, Category="Camera", meta=(ToolTip="Reference that camera looking at"))
    ACharacter* CameraTarget;

    UPROPERTY(EditAnywhere, Category="Camera", meta=(ToolTip="Camera following position off the PlayerCharacter"))
    float CameraOffset = 350.0f;

    UPROPERTY(EditAnywhere, Category="Camera", meta=(ToolTip="Height threshold when landed"))
    float ZHightThreshold = 200.0f;

    UPROPERTY(EditAnywhere, Category="Camera", meta=(ToolTip="Height threshold when falling"))
    float ZHightUpperThreshold = 350.0f;

    UPROPERTY(EditAnywhere, Category="Camera", meta=(ToolTip="Camera speed to a new position"))
    float InterpSpeed = 3.0f;

    UPROPERTY(EditAnywhere, Category="Camera")
    float SprintArmComponentSocketOffsetZ = 150.0f;

    UPROPERTY(EditAnywhere, Category="Camera", meta=(ToolTip="Camera starts following moving forward"))
    float ForwardDirectionDistanceMaxThreshold = 300.0f;

    UPROPERTY(EditAnywhere, Category="Camera", meta=(ToolTip="Camera stops following moving forward"))
    float ForwardDirectionDistanceMinThreshold = 50.0f;

    UPROPERTY(EditAnywhere, Category="Camera", meta=(ToolTip="Camera starts following moving backward"))
    float BackDirectionDistanceMaxThreshold = 900.0f;
    
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
    bool bIsCameraTracking = false;

    float GetZHeight() const;
    FVector GetCharacterOffset() const;
    void CheckTracking();
    bool CheckCameraDirection() const;

};
