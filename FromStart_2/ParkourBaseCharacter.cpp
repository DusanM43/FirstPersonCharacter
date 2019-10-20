// Fill out your copyright notice in the Description page of Project Settings.

#include "ParkourBaseCharacter.h"

/***TEMPPPPPPP**/
#include "DrawDebugHelpers.h"


AParkourBaseCharacter::AParkourBaseCharacter()
{
	playerController = GetController();

	// Spring arm and camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");

	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(20.f, 0, 60.f));
	SpringArm->TargetArmLength = 0.0f;
	SpringArm->bUsePawnControlRotation = true;

	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;



	// Wall 
	LeftWallDetection = CreateDefaultSubobject<USceneComponent>("LeftWallDetection");
	RightWallDetection = CreateDefaultSubobject<USceneComponent>("RightWallDetection");
	ForwardWallDetection = CreateDefaultSubobject<USceneComponent>("ForwardWallDetection");

	RightWallDetection->SetupAttachment(RootComponent);
	RightWallDetection->SetRelativeLocation(FVector(70.f, 0.f, 0.f));

	LeftWallDetection->SetupAttachment(RootComponent);
	LeftWallDetection->SetRelativeLocation(FVector(0.f, -70.f, 0.f));

	RightWallDetection->SetupAttachment(RootComponent);
	RightWallDetection->SetRelativeLocation(FVector(0.f, 70.f, 0.f));

	

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	fSprintSpeed = 900.f;
	fNormalSpeed = 600.f;
	fCrouchSpeed = 300.f;

	fWallRunDuration = 1.5f;
	fWallClimbDuration = 1.5f;

	fWallRunSpeed = 20000.f;
	fWallClimbSpeed = 400.f;

	fJumpFromWall = 0.25f;

	bIsMovingForward = false;
	bIsSliding = false;
	bIsCrouching = false;
	bIsWallRunning = false;
	bIsWallClimbing = false;
	bIsHanging = false;

	GetCharacterMovement()->SetPlaneConstraintEnabled(true);
}

// Called when the game starts or when spawned
void AParkourBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AParkourBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	fDeltaTime = DeltaTime;
}

// Called to bind functionality to input
void AParkourBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AParkourBaseCharacter::setTimerDelegateFunc(FTimerHandle& th, FName functionName, float time, bool repeat)
{
	FTimerDelegate tempDel;
	tempDel.BindUFunction(this, FName(functionName));
	GetWorldTimerManager().SetTimer(th, tempDel, time, repeat);
}


// Returns state of the character
bool AParkourBaseCharacter::IsLeftWallRunning()
{
	return (bLeftWallRun) ? true : false;
}

bool AParkourBaseCharacter::IsRightWallRunning()
{
	return (bRightWallRun) ? true : false;
}

bool AParkourBaseCharacter::IsSliding()
{
	return (bIsSliding) ? true : false;
}

bool AParkourBaseCharacter::IsCrouching()
{
	return (bIsCrouching) ? true : false;
}


// Checks if character can move (isn't sliding etc.)
bool AParkourBaseCharacter::CanMove()
{
	return (!bIsSliding && !bIsWallRunning && !bIsWallClimbing) ? true : false;
}

// Checks if character can sprint (isn't sliding or crouching)
bool AParkourBaseCharacter::CanSprint()
{
	return (bIsMovingForward && !bIsSliding && !bIsCrouching && !bIsWallRunning) ? true : false;
}

// Moves character along forward vector
void AParkourBaseCharacter::MoveForward(float amount)
{
	if (CanMove())
		AddMovementInput(GetActorForwardVector(), amount);
	amount > 0 ? bIsMovingForward = true : bIsMovingForward = false;
}

// Moves character along right vector
void AParkourBaseCharacter::MoveRight(float amount)
{
	if (CanMove())
		AddMovementInput(GetActorRightVector(), amount);
}

// Increases max walk speed to fSprintSpeed
void AParkourBaseCharacter::Sprint(float amount)
{
	if (amount == 1 && CanSprint())
		GetCharacterMovement()->MaxWalkSpeed = fSprintSpeed;
	else if (GetCharacterMovement()->MaxWalkSpeed == fSprintSpeed)
		GetCharacterMovement()->MaxWalkSpeed = fNormalSpeed;
}


// If slide enabled, and has enough speed will slide, else he will crouch
void AParkourBaseCharacter::CrouchSlide(float amount)
{
	// Stops slideing friction if the speed is lower then the max crouch speed
	if (!CanCrouch() && GetCharacterMovement()->Velocity.Size() < fCrouchSpeed)
		GetCharacterMovement()->BrakingDecelerationWalking = 2048.0f;

	// If key is pressed and isn't crouching crouch, if enough char has enough speed reduce "friction" for slide effect
	if (amount == 1) {
		if (CanCrouch()) {
			Crouch();
			if (GetCharacterMovement()->Velocity.Size() > fCrouchSpeed)
				GetCharacterMovement()->BrakingDecelerationWalking = 650.0f;
		}	
	}
	else 
		if (GetCharacterMovement()->Velocity.Size() < fCrouchSpeed) 
			UnCrouch();
}

// Depending on conditions does, jump, wall run or wall climb
void AParkourBaseCharacter::Parkour(float amount)
{
	/* If player is on the ground check if player should wall climb/run or jump
	Else checks if he can jump "mid-air" from a wall run or climb;*/
	if (!GetCharacterMovement()->IsFalling()) {
		tempJumpTimerCheck = 0.f;
		setTimerDelegateFunc(th_JumpCheck, "JumpCheck", fDeltaTime, true);
	}
	else
	{
		if (bCanJumpFromWallClimb)
		{
			//SetActorRotation(GetActorRotation() +  FRotator(0, 180, 0));
			//Camera->SetWorldRotation(Camera->GetComponentRotation()+ FRotator(0, 180, 0));
			AddControllerYawInput(360 / 2.5);
			LaunchCharacter((GetActorForwardVector() + FVector(0, 0, 1)) * 500, true, true);
			bCanJumpFromWallClimb = false;
		}
		else if (bCanJumpFromWallRun) {
			LaunchCharacter((GetActorForwardVector() + FVector(0, 0, 1)) * 500, true, true);
			bCanJumpFromWallRun = false;
		}
	}
}

void AParkourBaseCharacter::JumpCheck()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		FHitResult forwardHit;
		bool bForwardHit;
		FVector v_ForwardEndPoint = FVector(GetActorLocation() + GetActorForwardVector() * 200.f);
		bForwardHit = GetWorld()->LineTraceSingleByChannel(forwardHit, GetActorLocation(), v_ForwardEndPoint, ECC_Visibility);
		if (!bForwardHit)
		{
			UE_LOG(LogTemp, Warning, TEXT("Jump"));
			Super::Jump();
		}
		/*else
		{
			// Wall Climb
			UE_LOG(LogTemp, Warning, TEXT("Wall Climb Start"));
			bIsWallClimbing = true;
			tempWallClimbDuration = 0.f;
			GetWorldTimerManager().ClearTimer(th_JumpCheck);
			setTimerDelegateFunc(th_WallClimb, "WallClimb", fDeltaTime, true);
		}*/
	}
	//Wall run
	else if (GetCharacterMovement()->IsFalling() && GetCharacterMovement()->Velocity.Size() < fNormalSpeed)
	{
		FHitResult hr_Right;
		FHitResult hr_Left;

		bool bRightHit;
		bool bLeftHit;

		v_RightWallRun = RightWallDetection->GetComponentLocation();
		v_LeftWallRun = LeftWallDetection->GetComponentLocation();

		// Checks if there is a wall in both directions
		bRightHit = GetWorld()->LineTraceSingleByChannel(hr_Right, GetActorLocation(), v_RightWallRun, ECC_Visibility);
		bLeftHit = GetWorld()->LineTraceSingleByChannel(hr_Left, GetActorLocation(), v_LeftWallRun, ECC_Visibility);

		// Wall Run
		if (bRightHit || bLeftHit)
		{
			LaunchCharacter(FVector(0, 0, 0), true, true);
			UE_LOG(LogTemp, Warning, TEXT("Wall R Start"));
			bIsWallRunning = true;
			tempWallRunDuration = 0.f;
			GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 0, 1));
			GetCharacterMovement()->GravityScale = 0;
			GetWorldTimerManager().ClearTimer(th_JumpCheck);
			FTimerDelegate td_WallRun;
			if (bRightHit)
			{
				td_WallRun.BindUFunction(this, FName("WallRun"), RightWallDetection);

				Camera->AddRelativeRotation(FRotator(0, 0, -20));

				bRightWallRun = true;
			}
			else {
				td_WallRun.BindUFunction(this, FName("WallRun"), LeftWallDetection);

				Camera->AddRelativeRotation(FRotator(0, 0, 20));

				bLeftWallRun = true;
			}
			GetWorldTimerManager().SetTimer(th_WallRun, td_WallRun, fDeltaTime, true);
			if (GetWorldTimerManager().IsTimerActive(th_JumpCheck))
				GetWorldTimerManager().ClearTimer(th_JumpCheck);
		}
	}
	if (tempJumpTimerCheck > fJumpTimerCheck)
		GetWorldTimerManager().ClearTimer(th_JumpCheck);
	tempJumpTimerCheck += fDeltaTime;

}

void AParkourBaseCharacter::WallClimb()
{
	UE_LOG(LogTemp, Warning, TEXT("Wall Climb Repeat"));
	FHitResult forwardHit;
	bool bForwardHit;
	FVector v_ForwardEndPoint = FVector(GetActorLocation() + GetActorForwardVector() * 200.f);
	bForwardHit = GetWorld()->LineTraceSingleByChannel(forwardHit, GetActorLocation(), v_ForwardEndPoint, ECC_Visibility);
	DrawDebugLine(GetWorld(), GetActorLocation(), v_ForwardEndPoint, FColor::Green, false, 5, 0, 5);
	if (bForwardHit && tempWallClimbDuration < fWallClimbDuration)
	{
		LaunchCharacter(FVector(0, 0, 1) * fWallClimbSpeed, true, true);
	}
	else
	{
		WallClimbEnd();
	}
	tempWallClimbDuration += fDeltaTime;
}

void AParkourBaseCharacter::WallClimbEnd()
{
	GetWorldTimerManager().ClearTimer(th_WallClimb);
	bIsWallClimbing = false;
	bCanJumpFromWallClimb = true;
	setTimerDelegateFunc(th_CanJumpFromWall, FName("TimerCanJumpFromWall"), 0.5f, false);
}


// Wall Run Function, checks if the character is still next to wall and if he can still run because of the duration /
void AParkourBaseCharacter::WallRun(USceneComponent* wallDetector)
{
	UE_LOG(LogTemp, Warning, TEXT("Wall Run Repeat"));
	FVector forceDirection = (wallDetector == RightWallDetection) ? FVector().RightVector : FVector().LeftVector;
	FHitResult hr_Side;
	bool bSideHit;	
	bSideHit = GetWorld()->LineTraceSingleByChannel(hr_Side, GetActorLocation(), wallDetector->GetComponentLocation(), ECC_Visibility);
	
	
	if (bSideHit && tempWallRunDuration < fWallRunDuration)
	{
		//DrawDebugLine(GetWorld(), GetActorLocation(), hr_Side.Location, FColor::Green, false, 5, 0, 5);
		DrawDebugLine(GetWorld(), hr_Side.Location, hr_Side.Location + forceDirection, FColor::Green, true, 5, 0, 2);
		GetCharacterMovement()->AddForce((hr_Side.Location + forceDirection) *fWallRunSpeed); // GetActorForwardVector() * fWallRunSpeed);
	}
	else
	{
		WallRunEnd();
	}
	tempWallRunDuration += fDeltaTime;
}

void AParkourBaseCharacter::WallRunEnd()
{
	Camera->SetRelativeRotation(FRotator(0, 0, 0));
	GetCharacterMovement()->GravityScale = 1;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 0, 0));
	GetWorldTimerManager().ClearTimer(th_WallRun);
	bIsWallRunning = bRightWallRun = bLeftWallRun = false;
	bCanJumpFromWallRun = true;
	setTimerDelegateFunc(th_CanJumpFromWall, FName("TimerCanJumpFromWall"), fJumpFromWall, false);
}


void AParkourBaseCharacter::UnParkour()
{

	//Camera->SetRelativeRotation(FRotator(0, 0, 0));

	if (GetWorldTimerManager().IsTimerActive(th_WallRun))
		WallRunEnd();
	if (GetWorldTimerManager().IsTimerActive(th_WallClimb))
		WallClimbEnd();
}

void AParkourBaseCharacter::TimerCanJumpFromWall()
{
	bCanJumpFromWallClimb = false;
	bCanJumpFromWallRun = false;
	if (GetWorldTimerManager().IsTimerActive(th_CanJumpFromWall))
		GetWorldTimerManager().ClearTimer(th_CanJumpFromWall);
}

