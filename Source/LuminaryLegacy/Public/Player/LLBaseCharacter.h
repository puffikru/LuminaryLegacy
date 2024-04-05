// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LLBaseCharacter.generated.h"

class ALLPlayerCamera;
enum class EViewType : uint8;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class LUMINARYLEGACY_API ALLBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALLBaseCharacter();

    UPROPERTY(EditAnywhere)
    bool SideWalk = false;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
    USpringArmComponent* TPSpringArmComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
    UCameraComponent* TPCameraComponent;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
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
    void MoveForward(float Value);
    void MoveRight(float Value);

    // Camera
    void SwitchCamera();
    void SetView(EViewType View, float BlendTime = 0.0f);
    void SetupCamera(EViewType View);

    // float BlendTime = 1.0f;
};
