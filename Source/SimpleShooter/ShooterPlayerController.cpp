// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"

void AShooterPlayerController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);

	HUD->RemoveFromViewport();
	if (bIsWinner)
	{
		AddWidgetToViewport(WinScreenClass);
	}
	else 
	{
		AddWidgetToViewport(LoseScreenClass);
	}
	
	GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	HUD = CreateWidget(this, HUDClass);
	if (HUD != nullptr)
	{
		HUD->AddToViewport();
	}
}

void AShooterPlayerController::AddWidgetToViewport(TSubclassOf<UUserWidget> WidgetClass)
{
	UUserWidget* Screen = CreateWidget(this, WidgetClass);
	if (Screen != nullptr)
	{
		Screen->AddToViewport();
	}
}
