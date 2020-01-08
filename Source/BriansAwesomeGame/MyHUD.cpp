// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"
#include "Components/TimelineComponent.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "HealthParentWidget.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

AMyHUD::AMyHUD() {}

void AMyHUD::BeginPlay()
{
	FString deathMessage = FString::Printf(TEXT("HUD BEGIN PLAY"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, deathMessage);
	Super::BeginPlay();
	if (wBestHealthBar)
	{

		MyHealthBar = CreateWidget<UHealthParentWidget>(GetWorld(), wBestHealthBar);
		/** Make sure widget was created */
		if (MyHealthBar)
		{
			/** Add it to the viewport */
			MyHealthBar->AddToViewport();
			deathMessage = FString::Printf(TEXT("adding to viewport"));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, deathMessage);
		} else { 
			deathMessage = FString::Printf(TEXT("NOTadding to viewport")); 
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, deathMessage);
		}
	}
}

void AMyHUD::DrawHUD()
{
	Super::DrawHUD();
}