// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Components/AudioComponent.h"
#include "FighterAssignmentCharacter.h"
#include "FighterAssignmentGameMode.generated.h"

UCLASS(minimalapi)
class AFighterAssignmentGameMode : public AGameModeBase
{
	GENERATED_BODY()

	FTimerHandle MatchStartTimerHandle;
	FTimerHandle RoundStartTimerHandle;
	FTimerHandle RoundEndTimerHandle;
	FTimerHandle FightTextTimerHandle;
	FTimerHandle MatchEndTimerHandle;


public:
	AFighterAssignmentGameMode();

	virtual void StartPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	

	void MonitorHealth();
	void MonitorRounds();
	void RoundStart();
	void RoundEnd();
	void MatchEnd();
	void BeginFighting();
	void DisableFightText();
	void RunRoundTimer();
	void ReturnToTitle();

	UFUNCTION(BlueprintCallable)
		void MatchStart();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player References")
		AFighterAssignmentCharacter* player1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player References")
		AFighterAssignmentCharacter* player2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UAudioComponent* BGM;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		int Round;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		int Player1Rounds;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		int Player2Rounds;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float timeRemaining;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool roundStarting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool fightBegun = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool displayRoundText = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool displayFightText = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool displayP1WinText = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool displayP2WinText = false;
};
