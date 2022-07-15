// Copyright Epic Games, Inc. All Rights Reserved.

#include "FighterAssignmentCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

#define SAVEOUTTRANSFORMDATA 0	
#define IMPORTTRANSFORMDATA 0

void AFighterAssignmentCharacter::BeginPlay()
{
	Super::BeginPlay();

	TArray<UBoxComponent**> ColliderVars = {
		&BoxCollider5A,
		&BoxCollider5B,
		&BoxCollider5C,
		&BoxCollider2A,
		&BoxCollider2B,
		&BoxCollider2C,
		&BoxColliderjA,
		&BoxColliderjB,
		&BoxColliderjC,
		&BoxCollider236A,
		&BoxCollider236B,
		&BoxCollider236C,
		&BoxCollider214A,
		&BoxCollider214B,
		&BoxCollider214C,
		&BoxCollider4X
	};

	TArray<FString> ColliderNames = {
		"BoxColliderA5A",
		"BoxColliderA5B",
		"BoxColliderA5C",
		"BoxColliderA2A",
		"BoxColliderA2B",
		"BoxColliderA2C",
		"BoxColliderAjA",
		"BoxColliderAjB",
		"BoxColliderAjC",
		"BoxColliderA236A",
		"BoxColliderA236B",
		"BoxColliderA236C",
		"BoxColliderA214A",
		"BoxColliderA214B",
		"BoxColliderA214C",
		"BoxColliderA4X"
	};
	
	
	TArray<UActorComponent*> FoundComponents;
	FoundComponents = GetComponentsByClass(UBoxComponent::StaticClass());

	for (int i = 0; i < Commands::MAX_ATTACKS; i++)
	{
		FString ColliderName = FString::Printf(TEXT("BoxCollider%s"), *StaticEnum<Commands>()->GetDisplayNameTextByValue(Commands(i)).ToString());

		UE_LOG(LogTemp, Log, TEXT("Name we're looking for: %s"), *ColliderName);

		for (int j = 0; j < FoundComponents.Num(); j++)
		{
			UE_LOG(LogTemp, Log, TEXT("Name we are checking: %s"), *FoundComponents[j]->GetName());

			if (ColliderNames[i].Compare(FoundComponents[j]->GetName()) == 0)
			{
				UE_LOG(LogTemp, Log, TEXT("Name we found: %s"), *FoundComponents[j]->GetName());
				(*(ColliderVars[i])) = Cast<UBoxComponent>(FoundComponents[j]);
				(*(ColliderVars[i]))->OnComponentBeginOverlap.AddDynamic(this, &AFighterAssignmentCharacter::OnOverlapBegin);
				(*(ColliderVars[i]))->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				//BoxCollider->SetGenerateOverlapEvents(false);
				AttackColliderMap.Add(Commands(i), (*(ColliderVars[i])));
			}
		}
	}
	

#if SAVEOUTTRANSFORMDATA

	FString TransformString = FString();

	for (int i = 0; i < Commands::MAX_ATTACKS; i++)
	{
		UBoxComponent** Collider = AttackColliderMap.Find(Commands(i));

		if (Collider && *Collider)
		{
			TransformString += (*Collider)->GetRelativeTransform().ToString() + "\n";
		}
	}

	UE_LOG(LogTemp, Log, TEXT("%s"), *TransformString);
#endif

}

UBoxComponent* AFighterAssignmentCharacter::HitboxSwitch(int index)
{
	UBoxComponent* toReturn = nullptr;
	switch (index)
	{
	case 0:
		toReturn = BoxCollider5A;
		break;
	case 1:
		toReturn = BoxCollider5B;
		break;
	case 2:
		toReturn = BoxCollider5C;
		break;
	case 3:
		toReturn = BoxCollider2A;
		break;
	case 4:
		toReturn = BoxCollider2B;
		break;
	case 5:
		toReturn = BoxCollider2C;
		break;
	case 6:
		toReturn = BoxColliderjA;
		break;
	case 7:
		toReturn = BoxColliderjB;
		break;
	case 8:
		toReturn = BoxColliderjC;
		break;
	case 9:
		toReturn = BoxCollider236A;
		break;
	case 10:
		toReturn = BoxCollider236B;
		break;
	case 11:
		toReturn = BoxCollider236C;
		break;
	case 12:
		toReturn = BoxCollider214A;
		break;
	case 13:
		toReturn = BoxCollider214B;
		break;
	case 14:
		toReturn = BoxCollider214C;
		break;
	case 15:
		toReturn = BoxCollider4X;
		break;
	}

	return toReturn;
}

void AFighterAssignmentCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateFacingDirection();

	FighterStateManager();

}

void AFighterAssignmentCharacter::UpdateFacingDirection()
{
	FVector Forward = GetActorForwardVector();
	const FVector Right = FVector(0.0f, 0.0f, 90.0f);
	const FVector Left = FVector(0.0f, 0.0f, -90.0f);
	const FVector Front = FVector(0.0f, 0.0f, 0.0f);

	// Calculate which way character is facing
	PlayerDirection dir = Forward.Y < 0 ? PlayerDirection::RIGHT : PlayerDirection::LEFT;

	// Me face new direction, other guy over there
	if (OtherPlayer)
	{
		if (gameOver && isWinner)
		{
			SetActorRelativeRotation(FQuat::MakeFromEuler(Front));
		}
		else if ((OtherPlayer->GetTransform().GetLocation().Y - GetTransform().GetLocation().Y) * dir < 0 && isGrounded)
		{
			if (dir == PlayerDirection::RIGHT)
			{
				SetActorRelativeRotation(FQuat::MakeFromEuler(Right));
				onLeftSide = false;
			}
			else
			{
				SetActorRelativeRotation(FQuat::MakeFromEuler(Left));
				onLeftSide = true;
			}
		}
	}

}

void AFighterAssignmentCharacter::CheckGrounded()
{
	isGrounded = GetTransform().GetLocation().Z < 206.0f ? true : false;
	if (isGrounded)
	{
		Pushbox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
		//Pushbox->SetCollisionResponseToChannel(ECollisionChannel::, ECollisionResponse::ECR_Block);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		Pushbox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AFighterAssignmentCharacter::CheckBlocking()
{
	if ((GetTransform().GetRotation().Z < 0.0f && directionalInput == EDirectionalInput::VE_MovingLeft) ||
			(GetTransform().GetRotation().Z > 0.0f && directionalInput == EDirectionalInput::VE_MovingRight))
	{
		isBlocking = true;
	}
	else
	{
		isBlocking = false;
	}
}

AFighterAssignmentCharacter::AFighterAssignmentCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Face in the direction we are moving..
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 1400.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 2.5f;
	GetCharacterMovement()->MaxAcceleration = 50000.f;
	GetCharacterMovement()->AirControl = 0.0f;
	GetCharacterMovement()->AirControlBoostMultiplier = 0.0f;
	GetCharacterMovement()->AirControlBoostVelocityThreshold = 0.0f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 10000.f;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 0.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 300.f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	onLeftSide = false;

	lightAttackUsed = false;
	mediumAttackUsed = false;
	heavyAttackUsed = false;

	directionalInput = EDirectionalInput::VE_Default;

	OtherPlayer = nullptr;
	playerHealth = 100.00f;
	
	FString TransformString = FString();


	/*if (true)
	{
		FString ColliderName = FString::Printf(TEXT("BoxColliderNewCollider%s"), *StaticEnum<Commands>()->GetDisplayNameTextByValue(Commands(0)).ToString());
		BoxCollider5A = CreateDefaultSubobject<UBoxComponent>(FName(ColliderName));
		BoxCollider5A->AttachTo(RootComponent);
	}
	UE_LOG(LogTemp, Log, TEXT("We created the 5a collider %s"), *BoxCollider5A->GetName());

	for (int i = 1; i < Commands::MAX_ATTACKS; i++)
	{
		FString ColliderName = FString::Printf(TEXT("BoxColliderNewCollider%s"), *StaticEnum<Commands>()->GetDisplayNameTextByValue(Commands(i)).ToString());
		UBoxComponent* TempComponent = CreateDefaultSubobject<UBoxComponent>(FName(ColliderName));

		TempComponent->SetupAttachment(RootComponent);

		*(ColliderVars[i]) = TempComponent;
		UE_LOG(LogTemp, Log, TEXT("We created a collider %s"), *(*(ColliderVars[i]))->GetName())
	}*/

	

	Pushbox = CreateDefaultSubobject<UBoxComponent>(FName("Pushbox"));
	Pushbox->SetupAttachment(RootComponent);
	Pushbox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	static ConstructorHelpers::FObjectFinder<USoundWave> hitLight(TEXT("/Game/Sound/28H"));
	static ConstructorHelpers::FObjectFinder<USoundWave> hitHeavy(TEXT("/Game/Sound/2AH"));
	static ConstructorHelpers::FObjectFinder<USoundWave> blocked(TEXT("/Game/Sound/26H"));
	static ConstructorHelpers::FObjectFinder<USoundWave> knocked(TEXT("/Game/Sound/2EH"));

	HitLightSE = CreateDefaultSubobject<UAudioComponent>(TEXT("HitLightSound"));
	HitHeavySE = CreateDefaultSubobject<UAudioComponent>(TEXT("HitHeavySound"));
	BlockSE = CreateDefaultSubobject<UAudioComponent>(TEXT("BlockSound"));
	KnockedSE = CreateDefaultSubobject<UAudioComponent>(TEXT("KnockedSound"));

	HitLightSE->SetupAttachment(RootComponent);
	HitHeavySE->SetupAttachment(RootComponent);
	BlockSE->SetupAttachment(RootComponent);
	KnockedSE->SetupAttachment(RootComponent);

	HitLightSE->bAutoActivate = false;
	HitHeavySE->bAutoActivate = false;
	BlockSE->bAutoActivate = false;
	KnockedSE->bAutoActivate = false;

	//HitLightSE->Activate();

	if (hitLight.Succeeded())
	{
		HitLightSE->SetSound(hitLight.Object);
		HitHeavySE->SetSound(hitHeavy.Object);
		BlockSE->SetSound(blocked.Object);
		KnockedSE->SetSound(knocked.Object);
		//HitLightSE->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
		//HitLightSE->SetRelativeScale3D(FVector(1000.f, 1000.f, 1000.f));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("hitLight SE mapping failed"));
	}


#if IMPORTTRANSFORMDATA

	TArray<FString> TransformStrings = {
		"-300.000000,0.000000,0.000000 | 0.000000,0.000000,0.000000 | 1.000000,1.000000,1.000000",
		"0.000000,0.000000,0.000000 | 0.000000,0.000000,0.000000 | 1.000000,1.000000,1.000000",
		"0.000000,0.000000,0.000000 | 0.000000,0.000000,0.000000 | 1.000000,1.000000,1.000000",
		"0.000000,0.000000,0.000000 | 0.000000,0.000000,0.000000 | 1.000000,1.000000,1.000000",
		"0.000000,0.000000,0.000000 | 0.000000,0.000000,0.000000 | 1.000000,1.000000,1.000000",
		"0.000000,0.000000,0.000000 | 0.000000,0.000000,0.000000 | 1.000000,1.000000,1.000000",
		"0.000000,0.000000,0.000000 | 0.000000,0.000000,0.000000 | 1.000000,1.000000,1.000000",
		"0.000000,0.000000,0.000000 | 0.000000,0.000000,0.000000 | 1.000000,1.000000,1.000000",
		"0.000000,0.000000,0.000000 | 0.000000,0.000000,0.000000 | 1.000000,1.000000,1.000000",
		"0.000000,0.000000,0.000000 | 0.000000,0.000000,0.000000 | 1.000000,1.000000,1.000000",
		"0.000000,0.000000,0.000000 | 0.000000,0.000000,0.000000 | 1.000000,1.000000,1.000000",
		"0.000000,0.000000,0.000000 | 0.000000,0.000000,0.000000 | 1.000000,1.000000,1.000000",
		"0.000000,0.000000,0.000000 | 0.000000,0.000000,0.000000 | 1.000000,1.000000,1.000000",
		"0.000000,0.000000,0.000000 | 0.000000,0.000000,0.000000 | 1.000000,1.000000,1.000000",
		"0.000000,0.000000,0.000000 | 0.000000,0.000000,0.000000 | 1.000000,1.000000,1.000000",
		"0.000000,0.000000,0.000000 | 0.000000,0.000000,0.000000 | 1.000000,1.000000,1.000000"
	};

	for (int i = 0; i < Commands::MAX_ATTACKS; i++)
	{
		UBoxComponent** Collider = AttackColliderMap.Find(Commands(i));

		if (Collider && *Collider)
		{
			if (TransformStrings.IsValidIndex(i))
			{
				FTransform NewTransform;
				NewTransform.InitFromString(TransformStrings[i]);
				(*Collider)->SetRelativeTransform(NewTransform);
			}
		}
	}

#endif

}

void AFighterAssignmentCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (OtherActor != this) 
	{
		AFighterAssignmentCharacter* otherFighter = Cast<AFighterAssignmentCharacter>(OtherActor);
		if (otherFighter)
		{
			// Apply damage to other person 
			otherFighter->GetHit();


			// Set attacker information (cancels etc)



		}
	}

	

}

//////////////////////////////////////////////////////////////////////////
// Input

void AFighterAssignmentCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFighterAssignmentCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFighterAssignmentCharacter::StopJumping);
	//PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFighterAssignmentCharacter::DoCrouch);
	//PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFighterAssignmentCharacter::UndoCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFighterAssignmentCharacter::StartCrouching);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFighterAssignmentCharacter::StopCrouching);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFighterAssignmentCharacter::MoveRight);

	PlayerInputComponent->BindAction("LightAttack", IE_Pressed, this, &AFighterAssignmentCharacter::ButtonA);
	//PlayerInputComponent->BindAction("LightAttack", IE_Released, this, &AFighterAssignmentCharacter::StopLightAttack);
	PlayerInputComponent->BindAction("MediumAttack", IE_Pressed, this, &AFighterAssignmentCharacter::ButtonB);
	//PlayerInputComponent->BindAction("MediumAttack", IE_Released, this, &AFighterAssignmentCharacter::StopMediumAttack);
	PlayerInputComponent->BindAction("HeavyAttack", IE_Pressed, this, &AFighterAssignmentCharacter::ButtonC);
	//PlayerInputComponent->BindAction("HeavyAttack", IE_Released, this, &AFighterAssignmentCharacter::StopHeavyAttack);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AFighterAssignmentCharacter::Dash);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &AFighterAssignmentCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AFighterAssignmentCharacter::TouchStopped);
}

void AFighterAssignmentCharacter::MoveRight(float Value)
{
	if (this)
	{
		if (Value > 0.20f)
		{
			directionalInput = EDirectionalInput::VE_MovingRight;
		}
		else if (Value < -0.20f)
		{
			directionalInput = EDirectionalInput::VE_MovingLeft;
		}
		else
		{
			directionalInput = EDirectionalInput::VE_Default;
		}
	}

	if (!isCrouching && !isSAttacking && !isCAttacking && !isDashing && state == FighterState::standing)
	{
		// add movement in that direction
		AddMovementInput(FVector(0.f, -1.f, 0.f), Value);
	}
}

void AFighterAssignmentCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// jump on any touch
	Jump();
}

void AFighterAssignmentCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	StopJumping();
}

void AFighterAssignmentCharacter::StartCrouching()
{
	if (isGrounded)
	{
		isCrouching = true;
	}
}

void AFighterAssignmentCharacter::StopCrouching()
{
	isCrouching = false;
}

void AFighterAssignmentCharacter::Jump()
{
	if (state == standing && ((!isSAttacking && !isCAttacking) || canCancelJump))
	{
		LowerDashingFlag();
		isJumping = true;
		attackCancelled = true;
		
		ACharacter::Jump();
		if (directionalInput == EDirectionalInput::VE_MovingRight)
		{
			GetCharacterMovement()->Velocity.Y = -300.0f;
		}
		else if (directionalInput == EDirectionalInput::VE_MovingLeft)
		{
			GetCharacterMovement()->Velocity.Y = 300.0f;
		}
		directionalInput = EDirectionalInput::VE_Jumping;
		SetFighterState(jumping);
	}

}

void AFighterAssignmentCharacter::StopJumping()
{
	ACharacter::StopJumping();
}

void AFighterAssignmentCharacter::Landed(const FHitResult& Hit)
{
	directionalInput = EDirectionalInput::VE_Default;
	isInJuggled = false;
	if (isInKnocked)
	{
		SetFighterState(knocked);
		KnockedDown();
		isJumping = false;
	}
	else
	{
		LowerDashingFlag();
		isJumping = false;
		SetFighterState(standing);
	}
}

//void AFighterAssignmentCharacter::DoCrouch()
//{
//	Crouch();
//	UE_LOG(LogTemp, Log, TEXT("crouch attempt"));
//}

//void AFighterAssignmentCharacter::UndoCrouch()
//{
//	UnCrouch();
//	UE_LOG(LogTemp, Log, TEXT("uncrouch attempt"));
//}

void AFighterAssignmentCharacter::FighterStateManager()
{
	CheckGrounded();
	CheckBlocking();

	if (isInJuggled)
	{
		state = FighterState::juggled;
	}
	else if (isInSHitstun)
	{
		state = FighterState::sHitstun;
	}
	else if (isInCHitstun)
	{
		state = FighterState::cHitstun;
	}
	else if (isInKnocked)
	{
		SetFighterState(knocked);
	}
	else if (isCounteringA || isCounteringB || isCounteringC)
	{
		state = FighterState::countering;
	}
	else if (isInSBlockstun)
	{
		state = FighterState::sBlockstun;
	}
	else if (isInCBlockstun)
	{
		state = FighterState::cBlockstun;
	}
	else if (isCrouching && !isSAttacking && !isInKnocked && isGrounded)
	{
		SetFighterState(crouching);
	}
	else if (isJumping)
	{
		SetFighterState(jumping);
	}

	else
	{
		SetFighterState(standing);
	}
}

void AFighterAssignmentCharacter::SetFighterState(FighterState newState)
{
	state = newState;
}

void AFighterAssignmentCharacter::SetFighterBool(int name, bool b)
{
	switch (name)
	{
	case 0:
		isGrounded = b;
		break;
	case 1:
		isSAttacking = b;
		break;
	case 2:
		isBlocking = b;
		break;
	case 3:
		isDashing = b;
		break;
	case 4:
		isInSHitstun = b;
		break;
	case 5:
		isInCHitstun = b;
		break;
	case 6:
		isInSBlockstun = b;
		break;
	case 7:
		isInCBlockstun = b;
		break;
	case 8:
		isInJuggled = b;
		if (b)
		{
			state = FighterState::juggled;
		}
		break;
	case 9:
		isInKnocked = b;
		break;
	case 10:
		isCounteringA = b;
		break;
	case 11:
		isCounteringB = b;
		break;
	case 12:
		isCounteringC = b;
		break;
	default:
		isGrounded = false;
		isSAttacking = false;
		isCAttacking = false;
		isBlocking = false;
		isDashing = false;
		isInSHitstun = false;
		isInCHitstun = false;
		isInSBlockstun = false;
		isInCBlockstun = false;
		isInJuggled = false;
		isInKnocked = false;
		isCounteringA = false;
		isCounteringB = false;
		isCounteringC = false;
	}
}

void AFighterAssignmentCharacter::TurnOnCurrentAttackHitbox()
{
	if (CurrentAttackProperties.attackName < Commands::MAX_ATTACKS)
	{
		if (UBoxComponent** CurrentColliderPtr = AttackColliderMap.Find(CurrentAttackProperties.attackName))
		{
			UBoxComponent* CurrentCollider = *CurrentColliderPtr;

			if (CurrentCollider)
			{
				CurrentCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				//CurrentCollider->SetHiddenInGame(false);

				UWorld* World = GetWorld();
				if (World)
				{
					//What happens when the timer expires
					FTimerDelegate TimerDelegate;
					TimerDelegate.BindUObject(this, &AFighterAssignmentCharacter::TurnOffCurrentAttackHitbox);

					//Does it loop?
					const bool bDoesTimerLoop = false;

					//Set the timer
					World->GetTimerManager().SetTimer(HitboxEndTimerHandle, TimerDelegate, (CurrentAttackProperties.hitboxEndTime - CurrentAttackProperties.hitboxStartupTime) / 60.0f, bDoesTimerLoop);

					UE_LOG(LogTemp, Warning, TEXT("Hitbox on"));
				}
			}
		}
	}
}

void AFighterAssignmentCharacter::TurnOffCurrentAttackHitbox()
{
	if (CurrentAttackProperties.attackName < Commands::MAX_ATTACKS)
	{
		if (UBoxComponent** CurrentColliderPtr = AttackColliderMap.Find(CurrentAttackProperties.attackName))
		{
			UBoxComponent* CurrentCollider = *CurrentColliderPtr;


			if (CurrentCollider)
			{
				CurrentCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				//CurrentCollider->SetHiddenInGame(true);
				UE_LOG(LogTemp, Warning, TEXT("Hitbox off"));

				UWorld* World = GetWorld();
				if (World)
				{
					//What happens when the timer expires
					FTimerDelegate TimerDelegate;
					TimerDelegate.BindUFunction(this, FName("LowerAttackingFlag"));

					//Does it loop?
					const bool bDoesTimerLoop = false;

					//Set the timer
					World->GetTimerManager().SetTimer(HitboxEndTimerHandle, TimerDelegate, CurrentAttackProperties.recoveryFrames / 60.0f, bDoesTimerLoop);

					UE_LOG(LogTemp, Warning, TEXT("Hitbox on"));
				}
			}
		}
	}
}

void AFighterAssignmentCharacter::TurnOffAllAttackHitboxes()
{
	if (BoxCollider5A && 
		BoxCollider5B &&
		BoxCollider5C &&
		BoxCollider2A &&
		BoxCollider2B &&
		BoxCollider2C &&
		BoxColliderjA &&
		BoxColliderjB &&
		BoxColliderjC &&
		BoxCollider236A &&
		BoxCollider236B &&
		BoxCollider236C &&
		BoxCollider214A &&
		BoxCollider214B &&
		BoxCollider214C &&
		BoxCollider4X)
	{
		BoxCollider5A->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxCollider5B->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxCollider5C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxCollider2A->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxCollider2B->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxCollider2C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxColliderjA->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxColliderjB->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxColliderjC->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxCollider236A->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxCollider236B->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxCollider236C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxCollider214A->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxCollider214B->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxCollider214C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxCollider4X->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to turn off hitboxes on colliders that don't exist yet"));
	}
}

void AFighterAssignmentCharacter::ResetActorTimeDilation()
{
	this->CustomTimeDilation = 1.0f;
	OtherPlayer->CustomTimeDilation = 1.0f;
}

void AFighterAssignmentCharacter::LowerAttackingFlag()
{
	TurnOffAllAttackHitboxes();
	isSAttacking = false;
	isCAttacking = false;
	canCancelA = false;
	canCancelB = false;
	canCancelC = false;
	canCancelJump = false;
	canCancelDash = false;
}

void AFighterAssignmentCharacter::LowerDashingFlag()
{
	if (isGrounded)
	{
		isDashing = false;
	}

	LowerAttackingFlag();
}

void AFighterAssignmentCharacter::SetAttack(FString attackName)
{
	FString ContextString;

	FAttackProperties* CurrentAttackPropertyPtr = AttackDataTable->FindRow<FAttackProperties>(FName(attackName), ContextString);
	if (CurrentAttackPropertyPtr)
	{
		CurrentAttackProperties = *CurrentAttackPropertyPtr;
	}
	else
	{
		CurrentAttackProperties.attackName = Commands::MAX_ATTACKS;
	}

	if (CurrentAttackProperties.attackName < Commands::MAX_ATTACKS)
	{
		StartAttack();
		//UE_LOG(LogTemp, Warning, TEXT(attackName));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AFighterAssignmentCharacter::StartLightAttack - Tried to start an attack that was not found in the data table"));
	}
}

void AFighterAssignmentCharacter::StartAttack()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(HitboxEndTimerHandle);
		if (state == FighterState::crouching || (state == FighterState::standing && isCrouching))
		{
			isCAttacking = true;
		}
		else
		{
			isSAttacking = true;
		}


		//What happens when the timer expires
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &AFighterAssignmentCharacter::TurnOnCurrentAttackHitbox);

		//Does it loop?
		const bool bDoesTimerLoop = false;

		//Set the timer
		World->GetTimerManager().SetTimer(HitboxStartTimerHandle, TimerDelegate, CurrentAttackProperties.hitboxStartupTime / 60.0f, bDoesTimerLoop);


		//Clear the timer if you want to abort it
		//World->GetTimerManager().ClearTimer(TempHandle);
	}
}

void AFighterAssignmentCharacter::ButtonA()
{
	if (canCancelA)
	{
		attackCancelled = true;
	}

	if ((state == FighterState::standing || state == FighterState::crouching) && !isCrouching && ((!isSAttacking && !isCAttacking) || canCancelA))
	{
		lightAttackUsed = true;

		FString ContextString;

		FAttackProperties* CurrentAttackPropertyPtr = AttackDataTable->FindRow<FAttackProperties>(FName("5A"), ContextString);
		if (CurrentAttackPropertyPtr)
		{
			CurrentAttackProperties = *CurrentAttackPropertyPtr;
		}
		else
		{
			CurrentAttackProperties.attackName = Commands::MAX_ATTACKS;
		}

		if (CurrentAttackProperties.attackName < Commands::MAX_ATTACKS)
		{
			StartAttack();
			UE_LOG(LogTemp, Warning, TEXT("5A"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AFighterAssignmentCharacter::ButtonA - Tried to start an attack that was not found in the data table"));
		}
	}
	else if ((state == FighterState::standing || state == FighterState::crouching) && isCrouching && ((!isSAttacking && !isCAttacking) || canCancelA))
	{
		lightAttackUsed = true;

		FString ContextString;

		FAttackProperties* CurrentAttackPropertyPtr = AttackDataTable->FindRow<FAttackProperties>(FName("2A"), ContextString);
		if (CurrentAttackPropertyPtr)
		{
			CurrentAttackProperties = *CurrentAttackPropertyPtr;
		}
		else
		{
			CurrentAttackProperties.attackName = Commands::MAX_ATTACKS;
		}

		if (CurrentAttackProperties.attackName < Commands::MAX_ATTACKS)
		{
			StartAttack();
			UE_LOG(LogTemp, Warning, TEXT("2A"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AFighterAssignmentCharacter::ButtonA - Tried to start an attack that was not found in the data table"));
		}
	}
	else if (state == FighterState::jumping && ((!isSAttacking && !isCAttacking) || canCancelA))
	{
		lightAttackUsed = true;

		FString ContextString;

		FAttackProperties* CurrentAttackPropertyPtr = AttackDataTable->FindRow<FAttackProperties>(FName("jA"), ContextString);
		if (CurrentAttackPropertyPtr)
		{
			CurrentAttackProperties = *CurrentAttackPropertyPtr;
		}
		else
		{
			CurrentAttackProperties.attackName = Commands::MAX_ATTACKS;
		}

		if (CurrentAttackProperties.attackName < Commands::MAX_ATTACKS)
		{
			StartAttack();
			UE_LOG(LogTemp, Warning, TEXT("jA"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AFighterAssignmentCharacter::ButtonA - Tried to start an attack that was not found in the data table"));
		}
	}
}

void AFighterAssignmentCharacter::ButtonB()
{
	if (canCancelB)
	{
		attackCancelled = true;
	}

	if ((state == FighterState::standing || state == FighterState::crouching) && !isCrouching && ((!isSAttacking && !isCAttacking) || canCancelB))
	{
		mediumAttackUsed = true;

		FString ContextString;

		FAttackProperties* CurrentAttackPropertyPtr = AttackDataTable->FindRow<FAttackProperties>(FName("5B"), ContextString);
		if (CurrentAttackPropertyPtr)
		{
			CurrentAttackProperties = *CurrentAttackPropertyPtr;
		}
		else
		{
			CurrentAttackProperties.attackName = Commands::MAX_ATTACKS;
		}

		if (CurrentAttackProperties.attackName < Commands::MAX_ATTACKS)
		{
			StartAttack();
			UE_LOG(LogTemp, Warning, TEXT("5B"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AFighterAssignmentCharacter::ButtonB - Tried to start an attack that was not found in the data table"));
		}
	}
	else if ((state == FighterState::standing || state == FighterState::crouching) && isCrouching && ((!isSAttacking && !isCAttacking) || canCancelB))
	{
		mediumAttackUsed = true;

		FString ContextString;

		FAttackProperties* CurrentAttackPropertyPtr = AttackDataTable->FindRow<FAttackProperties>(FName("2B"), ContextString);
		if (CurrentAttackPropertyPtr)
		{
			CurrentAttackProperties = *CurrentAttackPropertyPtr;
		}
		else
		{
			CurrentAttackProperties.attackName = Commands::MAX_ATTACKS;
		}

		if (CurrentAttackProperties.attackName < Commands::MAX_ATTACKS)
		{
			StartAttack();
			UE_LOG(LogTemp, Warning, TEXT("2B"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AFighterAssignmentCharacter::ButtonB - Tried to start an attack that was not found in the data table"));
		}
	}
	else if (state == FighterState::jumping && ((!isSAttacking && !isCAttacking) || canCancelB))
	{
		mediumAttackUsed = true;

		FString ContextString;

		FAttackProperties* CurrentAttackPropertyPtr = AttackDataTable->FindRow<FAttackProperties>(FName("jB"), ContextString);
		if (CurrentAttackPropertyPtr)
		{
			CurrentAttackProperties = *CurrentAttackPropertyPtr;
		}
		else
		{
			CurrentAttackProperties.attackName = Commands::MAX_ATTACKS;
		}

		if (CurrentAttackProperties.attackName < Commands::MAX_ATTACKS)
		{
			StartAttack();
			UE_LOG(LogTemp, Warning, TEXT("jB"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AFighterAssignmentCharacter::ButtonB - Tried to start an attack that was not found in the data table"));
		}
	}

	canCancelB = false;
}

void AFighterAssignmentCharacter::ButtonC()
{
	if (canCancelC)
	{
		attackCancelled = true;
	}

	if ((state == FighterState::standing || state == FighterState::crouching) && !isCrouching && ((!isSAttacking && !isCAttacking) || canCancelC))
	{
		heavyAttackUsed = true;


		FString ContextString;

		FAttackProperties* CurrentAttackPropertyPtr = AttackDataTable->FindRow<FAttackProperties>(FName("5C"), ContextString);
		if (CurrentAttackPropertyPtr)
		{
			CurrentAttackProperties = *CurrentAttackPropertyPtr;
		}
		else
		{
			CurrentAttackProperties.attackName = Commands::MAX_ATTACKS;
		}

		if (CurrentAttackProperties.attackName < Commands::MAX_ATTACKS)
		{
			StartAttack();
			UE_LOG(LogTemp, Warning, TEXT("5C"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AFighterAssignmentCharacter::ButtonC - Tried to start an attack that was not found in the data table"));
		}
	}
	else if ((state == FighterState::standing || state == FighterState::crouching) && isCrouching && ((!isSAttacking && !isCAttacking) || canCancelC))
	{
		heavyAttackUsed = true;

		FString ContextString;

		FAttackProperties* CurrentAttackPropertyPtr = AttackDataTable->FindRow<FAttackProperties>(FName("2C"), ContextString);
		if (CurrentAttackPropertyPtr)
		{
			CurrentAttackProperties = *CurrentAttackPropertyPtr;
		}
		else
		{
			CurrentAttackProperties.attackName = Commands::MAX_ATTACKS;
		}

		if (CurrentAttackProperties.attackName < Commands::MAX_ATTACKS)
		{
			StartAttack();
			UE_LOG(LogTemp, Warning, TEXT("2C"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AFighterAssignmentCharacter::ButtonC - Tried to start an attack that was not found in the data table"));
		}
	}
	else if (state == FighterState::jumping && ((!isSAttacking && !isCAttacking) || canCancelC))
	{
		heavyAttackUsed = true;

		FString ContextString;

		FAttackProperties* CurrentAttackPropertyPtr = AttackDataTable->FindRow<FAttackProperties>(FName("jC"), ContextString);
		if (CurrentAttackPropertyPtr)
		{
			CurrentAttackProperties = *CurrentAttackPropertyPtr;
		}
		else
		{
			CurrentAttackProperties.attackName = Commands::MAX_ATTACKS;
		}

		if (CurrentAttackProperties.attackName < Commands::MAX_ATTACKS)
		{
			StartAttack();
			UE_LOG(LogTemp, Warning, TEXT("jC"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AFighterAssignmentCharacter::ButtonC - Tried to start an attack that was not found in the data table"));
		}
	}

	canCancelC = false;
}

void AFighterAssignmentCharacter::Dash()
{
	if (this && !isDashing && (state == standing || state == jumping) && ((!isSAttacking && !isCAttacking) || canCancelDash))
	{
		EMovementMode currentMovementMode = this->GetCharacterMovement()->MovementMode;
		isDashing = true;
		attackCancelled = true;
		UE_LOG(LogTemp, Warning, TEXT("Start dash"));

		// Dash cooldown
		UWorld* World = GetWorld();

		//What happens when the timer expires
		FTimerDelegate CooldownDelegate;
		CooldownDelegate.BindUFunction(this, FName("LowerDashingFlag"));

		//Does it loop?
		const bool bDoesCooldownLoop = false;

		//Set the timer
		World->GetTimerManager().SetTimer(DashCooldownTimerHandle, CooldownDelegate, 15.0f / 60.0f, bDoesCooldownLoop);


		// Grounded dashes
		if (state == standing && currentMovementMode != EMovementMode::MOVE_Flying)
		{
			if (directionalInput == EDirectionalInput::VE_MovingRight && OtherPlayer->GetActorForwardVector().Y < 0)
			{
				this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
				this->GetCharacterMovement()->Velocity = (FVector(0.0f, -600.0f, 0.0f));

				World = GetWorld();

				//What happens when the timer expires
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindLambda([&, currentMovementMode]() {
					this->GetCharacterMovement()->SetMovementMode(currentMovementMode);
					});

				//Does it loop?
				const bool bDoesTimerLoop = false;

				//Set the timer
				World->GetTimerManager().SetTimer(DashTimerHandle, TimerDelegate, 8.0f / 60.0f, bDoesTimerLoop);

				UE_LOG(LogTemp, Warning, TEXT("Character dashed 1!"));
			}
			else if (OtherPlayer->GetActorForwardVector().Y < 0)
			{
				this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
				this->GetCharacterMovement()->Velocity = (FVector(0.0f, 800.0f, 0.0f));

				World = GetWorld();

				//What happens when the timer expires
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindLambda([&, currentMovementMode]() {
					this->GetCharacterMovement()->SetMovementMode(currentMovementMode);
					});

				//Does it loop?
				const bool bDoesTimerLoop = false;

				//Set the timer
				World->GetTimerManager().SetTimer(DashTimerHandle, TimerDelegate, 8.0f / 60.0f, bDoesTimerLoop);

				UE_LOG(LogTemp, Warning, TEXT("Character dashed 2!"));
			}
			if (directionalInput == EDirectionalInput::VE_MovingLeft && OtherPlayer->GetActorForwardVector().Y > 0)
			{
				this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
				this->GetCharacterMovement()->Velocity = (FVector(0.0f, 600.0f, 0.0f));

				World = GetWorld();

				//What happens when the timer expires
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindLambda([&, currentMovementMode]() {
					this->GetCharacterMovement()->SetMovementMode(currentMovementMode);
					});

				//Does it loop?
				const bool bDoesTimerLoop = false;

				//Set the timer
				World->GetTimerManager().SetTimer(DashTimerHandle, TimerDelegate, 8.0f / 60.0f, bDoesTimerLoop);

				UE_LOG(LogTemp, Warning, TEXT("Character dashed 3!"));
			}
			else if (OtherPlayer->GetActorForwardVector().Y > 0)
			{
				this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
				this->GetCharacterMovement()->Velocity = (FVector(0.0f, -800.0f, 0.0f));

				World = GetWorld();

				//What happens when the timer expires
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindLambda([&, currentMovementMode]() {
					this->GetCharacterMovement()->SetMovementMode(currentMovementMode);
					});

				//Does it loop?
				const bool bDoesTimerLoop = false;

				//Set the timer
				World->GetTimerManager().SetTimer(DashTimerHandle, TimerDelegate, 8.0f / 60.0f, bDoesTimerLoop);

				UE_LOG(LogTemp, Warning, TEXT("Character dashed 4!"));
			}
		}

		// Aerial dashes
		if (state == jumping)
		{
			if (directionalInput == EDirectionalInput::VE_MovingRight && OtherPlayer->GetActorForwardVector().Y < 0)
			{
				this->GetCharacterMovement()->Velocity = FVector(0.0f, -450.0f, 100.0f);

				UE_LOG(LogTemp, Warning, TEXT("Character air dashed 1!"));
			}
			else if (OtherPlayer->GetActorForwardVector().Y < 0)
			{
				this->GetCharacterMovement()->Velocity = FVector(0.0f, 600.0f, 300.0f);

				UE_LOG(LogTemp, Warning, TEXT("Character air dashed 2!"));
			}
			if (directionalInput == EDirectionalInput::VE_MovingLeft && OtherPlayer->GetActorForwardVector().Y > 0)
			{
				this->GetCharacterMovement()->Velocity = FVector(0.0f, 450.0f, 100.0f);


				UE_LOG(LogTemp, Warning, TEXT("Character air dashed 3!"));
			}
			else if (OtherPlayer->GetActorForwardVector().Y > 0)
			{
				this->GetCharacterMovement()->Velocity = FVector(0.0f, -600.0f, 300.0f);

				UE_LOG(LogTemp, Warning, TEXT("Character air dashed 4!"));
			}
		}
	}
}

bool AFighterAssignmentCharacter::CheckCounterHit()
{
	bool isCounterHit = false;
	if (CurrentAttackProperties.attackName < Commands::MAX_ATTACKS)
	{
		if (CurrentAttackProperties.canBeCounterHit == true && (isSAttacking || isCAttacking))
		{
			isCounterHit = true;
			UE_LOG(LogTemp, Warning, TEXT("COUNTER!"));
		}
	}
	return isCounterHit;
}

bool AFighterAssignmentCharacter::CheckLauncher(bool counterHit)
{
	bool isLauncher = false;
	if (OtherPlayer->CurrentAttackProperties.attackName < Commands::MAX_ATTACKS)
	{
		if (OtherPlayer->CurrentAttackProperties.launch ||
			OtherPlayer->CurrentAttackProperties.launchOnCounter && counterHit)
		{
			isLauncher = true;
		}
	}
	return isLauncher;
}


void AFighterAssignmentCharacter::TakeDamage(float damageAmount)
{
	UE_LOG(LogTemp, Error, TEXT("Character took %f points of damage!!!"), damageAmount);
	playerHealth -= damageAmount;

	if (playerHealth < 0.00f)
	{
		playerHealth = 0.00f;
	}
}

void AFighterAssignmentCharacter::TakeChip(float damageAmount)
{
	float chip = damageAmount / 9.0f;
	UE_LOG(LogTemp, Error, TEXT("Character took %f points of chip damage!!!"), chip);
	playerHealth -= chip;

	if (playerHealth < 0.00f)
	{
		playerHealth = 0.00f;
	}
}

void AFighterAssignmentCharacter::TakeKnockUp(float knockUp, bool launcher)
{
	if (this)
	{
		if (launcher || this->state == jumping || this->state == juggled)
		{
			this->GetCharacterMovement()->Launch(FVector(0.0f, 0.0f, 1000.0f));
			SetFighterState(juggled);
			UE_LOG(LogTemp, Warning, TEXT("                   Character knocked up!"));
		}
	}
}

void AFighterAssignmentCharacter::TakeKnockBack(float knockBack, bool launcher)
{
	if (this)
	{
		float knockY = OtherPlayer->GetActorForwardVector().Y > 0 ? OtherPlayer->CurrentAttackProperties.knockBack : -OtherPlayer->CurrentAttackProperties.knockBack;

		// If Z-axis force to be added
		if (launcher && isGrounded && !isBlocking)
		{
			this->GetCharacterMovement()->Launch(FVector(0.0f, knockY / 4.0f, OtherPlayer->CurrentAttackProperties.knockUp));
			SetFighterBool(8, true);
			SetFighterBool(9, true);
			UE_LOG(LogTemp, Warning, TEXT("                   Character launched up!"));
		}
		else if (this->state == jumping || this->state == juggled)
		{
			this->GetCharacterMovement()->Velocity = (FVector(0.0f, knockY / 2.0f, OtherPlayer->CurrentAttackProperties.knockUp / 2.0f));
			SetFighterBool(8, true);
			SetFighterBool(9, true);
			UE_LOG(LogTemp, Warning, TEXT("                   Character juggled up!"));
		}
		// No Z-axis force
		else if (this->state == standing || this->state == crouching || this->state == sHitstun || this->state == cHitstun)
		{
			EMovementMode currentMovementMode = EMovementMode::MOVE_Walking;
			this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
			this->GetCharacterMovement()->Velocity = (FVector(0.0f, knockY, 0.0f));

			UWorld* World = GetWorld();

			//What happens when the timer expires
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindLambda([&, currentMovementMode]() {
				this->GetCharacterMovement()->SetMovementMode(currentMovementMode); 
				});

			//Does it loop?
			const bool bDoesTimerLoop = false;

			//Set the timer
			World->GetTimerManager().SetTimer(KnockbackTimerHandle, TimerDelegate, 8.0f / 60.0f, bDoesTimerLoop);

			UE_LOG(LogTemp, Warning, TEXT("                   Character knocked back!"));

			if ((GetTransform().GetLocation().Y > 890.0f || GetTransform().GetLocation().Y < -890.0f) && GetTransform().GetLocation().Z < 105.0f)
			{
				EMovementMode attackerCurrentMovementMode = OtherPlayer->GetCharacterMovement()->MovementMode;
				OtherPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
				OtherPlayer->GetCharacterMovement()->Velocity = (FVector(0.0f, -knockY, 0.0f));

				//What happens when the timer expires
				FTimerDelegate OtherTimerDelegate;
				OtherTimerDelegate.BindLambda([&, attackerCurrentMovementMode]() {
					OtherPlayer->GetCharacterMovement()->SetMovementMode(attackerCurrentMovementMode);
					});

				//Does it loop?
				const bool bDoesOtherTimerLoop = false;

				//Set the timer
				World->GetTimerManager().SetTimer(CornerKnockbackTimerHandle, OtherTimerDelegate, 8.0f / 60.0f, bDoesOtherTimerLoop);
			}

		}
		else
		{
			EMovementMode currentMovementMode = this->GetCharacterMovement()->MovementMode;
			this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
			this->GetCharacterMovement()->Velocity = (FVector(0.0f, knockY, 0.0f));

			UWorld* World = GetWorld();

			//What happens when the timer expires
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindLambda([&, currentMovementMode]() {
				this->GetCharacterMovement()->SetMovementMode(currentMovementMode);
				});

			//Does it loop?
			const bool bDoesTimerLoop = false;

			//Set the timer
			World->GetTimerManager().SetTimer(KnockbackTimerHandle, TimerDelegate, 7.0f / 60.0f, bDoesTimerLoop);

			UE_LOG(LogTemp, Warning, TEXT("                   Character knocked back!"));
		}
	}
}

void AFighterAssignmentCharacter::TakeHitStun(float hitStun)
{
	if (this)
	{
		UWorld* World = GetWorld();

		if ((this->state == FighterState::standing) || this->state == FighterState::sHitstun || this->state == FighterState::jumping || this->state == FighterState::juggled)
		{
			//SetFighterBool(4, true);
			isInSHitstun = true;
			UE_LOG(LogTemp, Warning, TEXT("             Character sHitstunned!"));
			
			if (World)
			{
				//What happens when the timer expires
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindUFunction(this, FName("SetFighterBool"), 4, false);

				//Does it loop?
				const bool bDoesTimerLoop = false;

				//Set the timer
				World->GetTimerManager().SetTimer(HitstunTimerHandle, TimerDelegate, OtherPlayer->CurrentAttackProperties.hitStun / 60.0f, bDoesTimerLoop);
			}

		}
		else if ((this->state == FighterState::crouching) || this->state == FighterState::cHitstun)
		{
			isInCHitstun = true;
			UE_LOG(LogTemp, Warning, TEXT("             Character cHitstunned!"));

			if (World)
			{
				//What happens when the timer expires
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindUFunction(this, FName("SetFighterBool"), 5, false);

				//Does it loop?
				const bool bDoesTimerLoop = false;

				//Set the timer
				World->GetTimerManager().SetTimer(HitstunTimerHandle, TimerDelegate, OtherPlayer->CurrentAttackProperties.hitStun / 60.0f, bDoesTimerLoop);
			}
		}
	}
	
}

void AFighterAssignmentCharacter::TakeBlockStun(float blockStun)
{
	if (this)
	{
		UWorld* World = GetWorld();

		if ((this->state == FighterState::standing && isBlocking) || this->state == FighterState::sBlockstun)
		{
			isInSBlockstun = true;
			UE_LOG(LogTemp, Warning, TEXT("             Character sBlockstunned!"));

			if (World)
			{
				//What happens when the timer expires
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindUFunction(this, FName("SetFighterBool"), 6, false);

				//Does it loop?
				const bool bDoesTimerLoop = false;

				//Set the timer
				World->GetTimerManager().SetTimer(BlockstunTimerHandle, TimerDelegate, OtherPlayer->CurrentAttackProperties.blockStun / 60.0f, bDoesTimerLoop);
			}

		}
		else if ((this->state == FighterState::crouching && isBlocking) || this->state == FighterState::cBlockstun)
		{
			isInCBlockstun = true;
			UE_LOG(LogTemp, Warning, TEXT("             Character cBlockstunned!"));

			if (World)
			{
				//What happens when the timer expires
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindUFunction(this, FName("SetFighterBool"), 7, false);

				//Does it loop?
				const bool bDoesTimerLoop = false;

				//Set the timer
				World->GetTimerManager().SetTimer(BlockstunTimerHandle, TimerDelegate, OtherPlayer->CurrentAttackProperties.blockStun / 60.0f, bDoesTimerLoop);
			}
		}
	}
}

void AFighterAssignmentCharacter::GetHit()
{
	if (OtherPlayer && OtherPlayer->AttackDataTable && state != knocked)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->GetTimerManager().ClearTimer(HitboxStartTimerHandle);
			World->GetTimerManager().ClearTimer(HitstunTimerHandle);
			World->GetTimerManager().ClearTimer(BlockstunTimerHandle);
			World->GetTimerManager().ClearTimer(KnockedTimerHandle);
		}
		LowerDashingFlag();
		
		FAttackProperties attackProperty = OtherPlayer->CurrentAttackProperties;
		bool isCounterHit = CheckCounterHit();
		bool isLauncher = CheckLauncher(isCounterHit);

		// Set cancels
		if (attackProperty.button == A)
		{
			OtherPlayer->canCancelA = false;
			OtherPlayer->canCancelB = true;
			OtherPlayer->canCancelC = true;
			OtherPlayer->canCancelJump = false;
			OtherPlayer->canCancelDash = true;
		}
		else if (attackProperty.button == B)
		{
			OtherPlayer->canCancelA = false;
			OtherPlayer->canCancelB = false;
			OtherPlayer->canCancelC = true;
			OtherPlayer->canCancelJump = true;
			OtherPlayer->canCancelDash = true;
		}
		else if (attackProperty.button == C)
		{
			OtherPlayer->canCancelA = false;
			OtherPlayer->canCancelB = false;
			OtherPlayer->canCancelC = false;
			OtherPlayer->canCancelJump = true;
			OtherPlayer->canCancelDash = false;
		}

		// Hitstop logic
		if (World)
		{
			this->CustomTimeDilation = 0.0f;
			OtherPlayer->CustomTimeDilation = 0.0f;

			//What happens when the timer expires
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindUObject(this, &AFighterAssignmentCharacter::ResetActorTimeDilation);

			//Does it loop?
			const bool bDoesTimerLoop = false;

			//Set the timer
			World->GetTimerManager().SetTimer(HitstopTimerHandle, TimerDelegate, 6.0f / 60.0f, bDoesTimerLoop);
		}

		// Logic on hit
		if (!isBlocking || isSAttacking || isCAttacking || isDashing || state == sHitstun || state == cHitstun || state == jumping || state == juggled ||
				(state == standing && attackProperty.type == Type::LOW) ||
				(state == crouching && attackProperty.type == Type::HIGH))
		{
			UE_LOG(LogTemp, Warning, TEXT("      Character hit!"));
			isDashing = false;
			FString contextString;
			if (attackProperty.attackName < MAX_ATTACKS)
			{
				TakeDamage(attackProperty.damage);
				TakeHitStun(attackProperty.hitStun);
				//TakeKnockUp(attackProperty.knockUp, isLauncher);
				TakeKnockBack(attackProperty.knockBack, isLauncher);

				if (attackProperty.button == Button::A || (attackProperty.button == Button::B && !isLauncher))
				{
					HitLightSE->Activate(true);
					HitLightSE->Play(0.0f);
				}
				else if (attackProperty.button == Button::C || isLauncher)
				{
					HitHeavySE->Activate(true);
					HitHeavySE->Play(0.0f);
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("No attackProperty found: %d"), attackProperty.attackName);
			}
		}

		// Logic on block
		else if ((isBlocking && (state == standing || state == sBlockstun) && (attackProperty.type == Type::HIGH || attackProperty.type == Type::MID)) ||
		(isBlocking && (state == crouching || state == cBlockstun) && (attackProperty.type == Type::LOW || attackProperty.type == Type::MID)))
		{
			UE_LOG(LogTemp, Warning, TEXT("      Character blocked!"));
			isDashing = false;
			FString contextString;
			if (attackProperty.attackName < MAX_ATTACKS)
			{
				TakeChip(attackProperty.damage);
				TakeBlockStun(attackProperty.blockStun);
				TakeKnockBack(attackProperty.knockBack, isLauncher);

				BlockSE->Activate(true);
				BlockSE->Play(0.0f);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("No attackProperty found: %d"), attackProperty.attackName);
			}
		}
	}
	
}

void AFighterAssignmentCharacter::KnockedDown()
{
	UWorld* World = GetWorld();
	if (World)
	{
		//What happens when the timer expires
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("SetFighterBool"), 9, false);

		//Does it loop?
		const bool bDoesTimerLoop = false;

		//Set the timer
		World->GetTimerManager().SetTimer(KnockedTimerHandle, TimerDelegate, wakeUpTime / 60.0f, bDoesTimerLoop);

		KnockedSE->Activate(true);
		KnockedSE->Play(0.0f);
	}
}
