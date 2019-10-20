// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "TimerManager.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "ParkourBaseCharacter.generated.h"

UCLASS()
class FROMSTART_2_API AParkourBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AParkourBaseCharacter();

	/** Used for right wall detection*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Detectors", meta = (AllowPrivateAccess = "true"))
		USceneComponent* ForwardWallDetection;

	/** Used for left wall detection*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Detectors", meta = (AllowPrivateAccess = "true"))
		USceneComponent* LeftWallDetection;

	/** Used for right wall detection*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Detectors", meta = (AllowPrivateAccess = "true"))
		USceneComponent* RightWallDetection;	

	/** Spring arm*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* SpringArm;

	/** Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Detectors", meta = (AllowPrivateAccess = "true"))
		UCameraComponent* Camera;

private:

	// Character states
	bool bIsMovingForward;
	bool bIsSliding;
	bool bIsCrouching;
	bool bIsWallRunning;
	bool bRightWallRun;
	bool bLeftWallRun;
	bool bIsWallClimbing;
	bool bIsHanging;

	float fDeltaTime;
	float tempWallRunDuration;
	float tempWallClimbDuration;
	float tempJumpTimerCheck;

	bool bCanJumpFromWallRun;
	bool bCanJumpFromWallClimb;

	UFUNCTION()
		void TimerCanJumpFromWall();


	// Checks if caracter can preform the given task
	UFUNCTION()
		bool CanSprint();

	// Checks if character can move
	bool CanMove();

	// Used for crouch/slide function
	FTimerHandle th_WallRun;
	FTimerHandle th_JumpCheck;
	FTimerHandle th_WallClimb;
	FTimerHandle th_CanJumpFromWall;


	UFUNCTION()
		void JumpCheck();

	UFUNCTION()
		void WallRun(USceneComponent* wallDetector);

	UFUNCTION()
		void WallClimb();

	/** Sets up timer delagate function */
	void setTimerDelegateFunc(FTimerHandle& th, FName functionName, float time, bool repeat);


	/** Sets everything needed after the wall run*/
	void WallRunEnd();

	/** Sets everything needed after the wall climb*/
	void WallClimbEnd();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Speed during a sprint */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", Meta = (BlueprintProtected = "true"))
		float fSprintSpeed;

	/** Speed during a run */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", Meta = (BlueprintProtected = "true"))
		float fNormalSpeed;

	/** Speed during a crouch */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UCrouch", Meta = (BlueprintProtected = "true"))
		float fCrouchSpeed;

	/** Max duration of the wall run*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UWallRun", Meta = (BlueprintProtected = "true"))
		float fWallRunDuration;

	/** Max duration of the wall run*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UWallClimb", Meta = (BlueprintProtected = "true"))
		float fWallClimbDuration;

	/** Max duration of the wall run*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UWallRun", Meta = (BlueprintProtected = "true"))
		float fWallRunSpeed;

	/** Max duration of the wall run*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UWallClimb", Meta = (BlueprintProtected = "true"))
		float fWallClimbSpeed;

	/** How long should timer check if he can wall run*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", Meta = (BlueprintProtected = "true"))
		float fJumpTimerCheck;

	/** Set the end point of line that checks if there is something above */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWallRun", Meta = (BlueprintProtected = "true"))
		FVector v_LeftWallRun;

	/** Set the end point of line that checks if there is something above */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWallRun", Meta = (BlueprintProtected = "true"))
		FVector v_RightWallRun;

	/** Set the end point of line that checks if there is something above */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", Meta = (BlueprintProtected = "true"))
		FVector v_ForwardEndPoint;

	/** Time until he can't jump from wall */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", Meta = (BlueprintProtected = "true"))
		float fJumpFromWall;

public:
	AController* playerController;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Set characters max speed to sprint */
	UFUNCTION(BlueprintCallable, Category = "Ability")
		void Sprint(float amount);

	/** Moves character in the forward vector direction */
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void MoveForward(float amount);

	/** Moves character in the right vector direction */
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void MoveRight(float amount);

	/** Allows character to crouch and slide*/
	UFUNCTION(BlueprintCallable, Category = "Ability")
		void CrouchSlide(float amount);


	/** Jump, wall run and wall climb */
	UFUNCTION(BlueprintCallable, Category = "Ability")
		virtual void Parkour(float amount);

	/** Stops wall run and climb */
	UFUNCTION(BlueprintCallable, Category = "Ability")
		virtual void UnParkour();



	// Returns the stats

	/** Returns if the player is running on left wall*/
	UFUNCTION(BlueprintPure, Category = "Status")
		bool IsLeftWallRunning();

	/** Returns if the player is running on right wall*/
	UFUNCTION(BlueprintPure, Category = "Status")
		bool IsRightWallRunning();

	/** Returns if the player is sliding*/
	UFUNCTION(BlueprintPure, Category = "Status")
		bool IsSliding();

	/** Returns if the player is crouching*/
	UFUNCTION(BlueprintPure, Category = "Status")
		bool IsCrouching();
};