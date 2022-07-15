// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AttackProperties.generated.h"


UENUM(BlueprintType)
enum Commands {
	A5A,
	A5B,
	A5C,
	A2A,
	A2B,
	A2C,
	AjA,
	AjB,
	AjC,
	A236A,
	A236B,
	A236C,
	A214A,
	A214B,
	A214C,
	A4X,
	MAX_ATTACKS
};

UENUM(BlueprintType)
enum Type {
	HIGH,
	MID,
	LOW
};

UENUM(BlueprintType)
enum Button {
	A,
	B,
	C
};

USTRUCT()
struct FIGHTERASSIGNMENT_API FAttackProperties : public FTableRowBase
{
	GENERATED_BODY()


public:
	FAttackProperties();
	~FAttackProperties();



	UPROPERTY(EditAnywhere)
		TEnumAsByte<Commands> attackName;

	UPROPERTY(EditAnywhere)
		float damage = 0;

	UPROPERTY(EditAnywhere)
		float hitStun = 0;

	UPROPERTY(EditAnywhere)
		float blockStun = 0;

	UPROPERTY(EditAnywhere)
		float hitStop = 0;

	UPROPERTY(EditAnywhere)
		float knockUp = 0;

	UPROPERTY(EditAnywhere)
		float knockBack = 0;

	UPROPERTY(EditAnywhere)
		float meterGain = 0;

	UPROPERTY(EditAnywhere)
		bool launch = false;

	UPROPERTY(EditAnywhere)
		bool launchOnCounter = false;

	UPROPERTY(EditAnywhere)
		bool canBeCounterHit = true;

	UPROPERTY(EditAnywhere)
		bool normalCancelable = false;

	UPROPERTY(EditAnywhere)
		bool specialCancelable = false;

	UPROPERTY(EditAnywhere)
		float hitboxStartupTime = 0.0f;

	UPROPERTY(EditAnywhere)
		float hitboxEndTime = 0.0f;

	UPROPERTY(EditAnywhere)
		TEnumAsByte<Type> type;

	UPROPERTY(EditAnywhere)
		TEnumAsByte<Button> button;

	UPROPERTY(EditAnywhere)
		float recoveryFrames = 0.0f;
};