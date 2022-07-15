// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BaseGameInstance.generated.h"

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	VE_Default	UMETA(DisplayName = "Mannequin"),
	VE_Gideon	UMETA(DisplayName = "Gideon"),
	VE_Phase	UMETA(DisplayName = "Phase")
};

UCLASS()
class FIGHTERASSIGNMENT_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Player References")
		ECharacterClass P1CharacterClass;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Player References")
		ECharacterClass P2CharacterClass;
	
};
