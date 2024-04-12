// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LLBaseCharacter.generated.h"

struct FInputActionValue;
class ALLPlayerCamera;
enum class EViewType : uint8;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;

UCLASS()
class LUMINARYLEGACY_API ALLBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALLBaseCharacter();

    UPROPERTY(EditAnywhere, Category="Movement")
    float JumpZVelocity = 1200.0f;

    UPROPERTY(EditAnywhere, Category="Movement")
    float AirControl = 0.8f;

    UPROPERTY(EditAnywhere, Category="Movement")
    float GravityScale = 2.5f;

    UPROPERTY(EditAnywhere, Category="Enhanced Input")
    UInputAction* InputMoving;

    UPROPERTY(EditAnywhere, Category="Enhanced Input")
    UInputAction* CameraSwitch;

    UPROPERTY(EditAnywhere, Category="Enhanced Input")
    UInputAction* Look;

    UPROPERTY(EditAnywhere, Category="Enhanced Input")
    UInputAction* Jumping;

    UPROPERTY(EditAnywhere, Category="Movement")
    float RotationRateYaw = 800.0f;

    UPROPERTY(EditAnywhere, Category="Camera")
    float CameraBlendTime = 1.0f;

    UPROPERTY(EditAnywhere, Category="Enhanced Input")
    TSoftObjectPtr<UInputMappingContext> InputMappingContext;

protected:
    UPROPERTY(VisibleAnywhere, Category="Components")
    USpringArmComponent* TPSpringArmComponent;
    
    UPROPERTY(VisibleAnywhere, Category="Components")
    UCameraComponent* TPCameraComponent;
    
    UPROPERTY(EditAnywhere, Category="Camera")
    TSubclassOf<ALLPlayerCamera> PlayerCamera2D;

    // Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    // Simple moving

    void Moving(const FInputActionValue& Value);
    void Looking(const FInputActionValue& Value);
    
    bool SideWalk = false;

    void JumpThrough() const;
    
    // Camera
    void SwitchCamera();
    void SetView(EViewType View, float BlendTime = 0.0f);
    void SetupCamera(EViewType View);
    ALLPlayerCamera* CreateCamera() const;
    
    AActor* Camera;
};
