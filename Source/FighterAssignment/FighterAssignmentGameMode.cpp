// Copyright Epic Games, Inc. All Rights Reserved.

#include "FighterAssignmentGameMode.h"
#include "FighterAssignmentCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AFighterAssignmentGameMode::AFighterAssignmentGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/SideScrollerCPP/Blueprints/SideScrollerCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<USoundWave> bgm(TEXT("/Game/Sound/bgm"));

	BGM = CreateDefaultSubobject<UAudioComponent>(TEXT("BackgroundMusic"));

	BGM->SetupAttachment(RootComponent);

	/*BGM->bAutoActivate = true;

	if (bgm.Succeeded())
	{
		BGM->SetSound(bgm.Object);
	}*/
}

void AFighterAssignmentGameMode::StartPlay()
{
	FString OutError;
	UGameplayStatics::GetGameInstance(this)->CreateLocalPlayer(1, OutError, true);

	Super::StartPlay();


	//MatchStart();
	Round = 0;
}

void AFighterAssignmentGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MonitorHealth();
	RunRoundTimer();
}

void AFighterAssignmentGameMode::MonitorHealth()
{
	if (fightBegun)
	{
		if (player1->playerHealth <= 0.0f)
		{
			RoundEnd();
			Player2Rounds++;
		}
		if (player2->playerHealth <= 0.0f)
		{
			RoundEnd();
			Player1Rounds++;
		}
	}
}

void AFighterAssignmentGameMode::MonitorRounds()
{
	if (Player1Rounds >= 2 || Player2Rounds >= 2)
	{
		MatchEnd();
	}
	else
	{
		Round++;
		RoundStart();
	}
}

void AFighterAssignmentGameMode::RoundStart()
{
	timeRemaining = 99.0f;
	roundStarting = true;
	displayRoundText = true;
	player1->playerHealth = 100.0f;
	player2->playerHealth = 100.0f;
	player1->SetActorLocation(FVector(0.0f, 200.0f, 100.0f));
	player2->SetActorLocation(FVector(0.0f, -200.0f, 100.0f));

	player1->isCrouching = false;
	player2->isCrouching = false;

	UWorld* World = GetWorld();
	if (World)
	{
		//What happens when the timer expires
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &AFighterAssignmentGameMode::BeginFighting);

		//Does it loop?
		const bool bDoesTimerLoop = false;

		//Set the timer
		World->GetTimerManager().SetTimer(RoundStartTimerHandle, TimerDelegate, 3.0f, bDoesTimerLoop);

		//UE_LOG(LogTemp, Warning, TEXT("ROUND %s"), Round);
	}
}

void AFighterAssignmentGameMode::RoundEnd()
{
	fightBegun = false;
	player1->DisableInput((APlayerController*)player1->GetController());
	player2->DisableInput((APlayerController*)player2->GetController());
	player1->directionalInput = EDirectionalInput::VE_Default;
	player2->directionalInput = EDirectionalInput::VE_Default;

	UWorld* World = GetWorld();
	if (World)
	{
		//What happens when the timer expires
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &AFighterAssignmentGameMode::MonitorRounds);

		//Does it loop?
		const bool bDoesTimerLoop = false;

		//Set the timer
		World->GetTimerManager().SetTimer(RoundEndTimerHandle, TimerDelegate, 3.0f, bDoesTimerLoop);

		UE_LOG(LogTemp, Warning, TEXT("Round End"));
	}
}

void AFighterAssignmentGameMode::MatchStart()
{
	player1->DisableInput((APlayerController*)player1->GetController());
	player2->DisableInput((APlayerController*)player2->GetController());
	Round = 1;

	UWorld* World = GetWorld();
	if (World)
	{
		//What happens when the timer expires
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &AFighterAssignmentGameMode::RoundStart);

		//Does it loop?
		const bool bDoesTimerLoop = false;

		//Set the timer
		World->GetTimerManager().SetTimer(MatchStartTimerHandle, TimerDelegate, 2.0f, bDoesTimerLoop);

		UE_LOG(LogTemp, Warning, TEXT("Match Start"));
	}
}

void AFighterAssignmentGameMode::MatchEnd()
{
	player1->gameOver = true;
	player2->gameOver = true;
	if (Player1Rounds >= 2)
	{
		// Player 1 win stuff
		displayP1WinText = true;
		player1->isWinner = true;
	}
	else
	{
		// Player 2 win stuff
		displayP2WinText = true;
		player2->isWinner = true;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		//What happens when the timer expires
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &AFighterAssignmentGameMode::ReturnToTitle);

		//Does it loop?
		const bool bDoesTimerLoop = false;

		//Set the timer
		World->GetTimerManager().SetTimer(MatchEndTimerHandle, TimerDelegate, 8.0f, bDoesTimerLoop);

		//UE_LOG(LogTemp, Warning, TEXT("ROUND %s"), Round);
	}
}

void AFighterAssignmentGameMode::BeginFighting()
{
	fightBegun = true;
	roundStarting = false;
	displayRoundText = false;
	displayFightText = true;
	player1->EnableInput((APlayerController*)player1->GetController());
	player2->EnableInput((APlayerController*)player2->GetController());

	UWorld* World = GetWorld();
	if (World)
	{
		//What happens when the timer expires
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &AFighterAssignmentGameMode::DisableFightText);

		//Does it loop?
		const bool bDoesTimerLoop = false;

		//Set the timer
		World->GetTimerManager().SetTimer(FightTextTimerHandle, TimerDelegate, 1.0f, bDoesTimerLoop);

		UE_LOG(LogTemp, Warning, TEXT("Match Start"));
	}
}

void AFighterAssignmentGameMode::DisableFightText()
{
	displayFightText = false;
}

void AFighterAssignmentGameMode::RunRoundTimer()
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (fightBegun)
		{
			timeRemaining -= World->GetDeltaSeconds();
		}
	}
}

void AFighterAssignmentGameMode::ReturnToTitle()
{
	UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");
}