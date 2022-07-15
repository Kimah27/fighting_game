// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Character.h"
#include "AttackProperties.h"
#include "FighterAssignmentCharacter.generated.h"

class UDataTable;
class UBoxComponent;

UENUM(BlueprintType)
enum FighterState {
	standing,
	crouching,
	jumping,
	sHitstun,
	cHitstun,
	sBlockstun,
	cBlockstun,
	juggled,
	knocked,
	countering
};

UENUM(BlueprintType)
enum class EDirectionalInput : uint8
{
	VE_Default		UMETA(DisplayName = "NOT_MOVING"),
	VE_MovingRight	UMETA(DisplayName = "MOVING_RIGHT"),
	VE_MovingLeft	UMETA(DisplayName = "MOVING_LEFT"),
	VE_Jumping		UMETA(DisplayName = "JUMPING")
};

UCLASS(config=Game)
class AFighterAssignmentCharacter : public ACharacter
{
	GENERATED_BODY()

	enum PlayerDirection {
		RIGHT = -1,
		LEFT = 1
	};

	/** Side view camera */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class USpringArmComponent* CameraBoom;

	FTimerHandle HitboxStartTimerHandle;
	FTimerHandle HitboxEndTimerHandle;
	FTimerHandle BlockstunTimerHandle;
	FTimerHandle HitstunTimerHandle;
	FTimerHandle KnockbackTimerHandle;
	FTimerHandle CornerKnockbackTimerHandle;
	FTimerHandle DashTimerHandle;
	FTimerHandle DashCooldownTimerHandle;
	FTimerHandle KnockedTimerHandle;
	FTimerHandle HitstopTimerHandle;

	void CheckGrounded();
	void CheckBlocking();
	void ButtonA();
	void ButtonB();
	void ButtonC();
	void Dash();

protected:

	/** Called for side to side input */
	void MoveRight(float Val);

	/** Handle touch inputs. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Handle touch stop event. */
	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);

	//void DoCrouch();
	//void UndoCrouch();

	UBoxComponent* HitboxSwitch(int index);

	UFUNCTION(BlueprintCallable)
		void StartCrouching();

	UFUNCTION(BlueprintCallable)
		void StopCrouching();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	UFUNCTION(BlueprintCallable)
	void FighterStateManager();

	UFUNCTION(BlueprintCallable)
	void SetFighterState(FighterState state);

	UFUNCTION(BlueprintCallable)
	void SetFighterBool(int name, bool b);

	UFUNCTION(BlueprintCallable)
	void LowerAttackingFlag();

	UFUNCTION(BlueprintCallable)
	void LowerDashingFlag();

	void SetAttack(FString attackName);

	void StartAttack();
	void TurnOnCurrentAttackHitbox();
	void TurnOffCurrentAttackHitbox();
	void TurnOffAllAttackHitboxes();
	void ResetActorTimeDilation();

	// Damage player
	bool CheckCounterHit();
	bool CheckLauncher(bool counterHit);
	void TakeDamage(float damageAmount);
	void TakeChip(float damageAmount);
	void TakeKnockUp(float knockUp, bool launcher);
	void TakeKnockBack(float knockBack, bool launcher);
	void TakeHitStun(float hitStun);
	void TakeBlockStun(float blockStun);

	void GetHit();
	void KnockedDown();



	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void UpdateFacingDirection();

	virtual void Jump() override;
	virtual void StopJumping() override;
	virtual void Landed(const FHitResult& Hit) override;
	

	UPROPERTY()
		TMap<TEnumAsByte<Commands>, UBoxComponent*> AttackColliderMap;

public:
	AFighterAssignmentCharacter();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Returns SideViewCameraComponent subobject **/
	//FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	//FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		TEnumAsByte<FighterState> state;

	UPROPERTY(VisibleAnywhere, Category = "FighterState")
		FAttackProperties CurrentAttackProperties;

	// Current player health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		float playerHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		float wakeUpTime = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool isGrounded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool isCrouching;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool isJumping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool isSAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool isCAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool isBlocking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool isDashing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool isInSHitstun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool isInCHitstun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool isInSBlockstun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool isInCBlockstun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool isInJuggled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool isInKnocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool isCounteringA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool isCounteringB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool isCounteringC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool canCancelA = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool canCancelB = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool canCancelC = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool canCancelJump = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FighterState")
		bool canCancelDash = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveList")
		int PlayerNum = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player References")	
		AFighterAssignmentCharacter* OtherPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveList")
		UDataTable* AttackDataTable = nullptr;

	// Has the player cancelled their attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacks")
		bool attackCancelled = false;

	// Has the player used the light attack?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacks")
		bool lightAttackUsed;

	// Has the player used the medium attack?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacks")
		bool mediumAttackUsed;

	// Has the player used the heavy attack?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacks")
		bool heavyAttackUsed;

	// Is the player on the left side?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool onLeftSide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match")
		bool isWinner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match")
		bool isLoser;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match")
		bool gameOver;

	// The direction the character is moving or direction the player is holding
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		EDirectionalInput directionalInput;

private:

	UPROPERTY(VisibleAnywhere, Category = "Hitboxes", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoxCollider5A;

	UPROPERTY(VisibleAnywhere, Category = "Hitboxes", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoxCollider5B;

	UPROPERTY(VisibleAnywhere, Category = "Hitboxes", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoxCollider5C;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hitboxes", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoxCollider2A;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hitboxes", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoxCollider2B;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hitboxes", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoxCollider2C;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hitboxes", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoxColliderjA;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hitboxes", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoxColliderjB;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hitboxes", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoxColliderjC;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hitboxes", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoxCollider236A;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hitboxes", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoxCollider236B;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hitboxes", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoxCollider236C;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hitboxes", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoxCollider214A;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hitboxes", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoxCollider214B;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hitboxes", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoxCollider214C;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hitboxes", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoxCollider4X;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* Pushbox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Effects", meta = (AllowPrivateAccess = "true"))
		UAudioComponent* HitLightSE;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sound Effects", meta = (AllowPrivateAccess = "true"))
		UAudioComponent* HitHeavySE;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sound Effects", meta = (AllowPrivateAccess = "true"))
		UAudioComponent* BlockSE;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sound Effects", meta = (AllowPrivateAccess = "true"))
		UAudioComponent* KnockedSE;
};
