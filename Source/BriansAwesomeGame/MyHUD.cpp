// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"
#include "Components/TimelineComponent.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"

AMyHUD::AMyHUD() {}

void AMyHUD::BeginPlay()
{
	Super::BeginPlay();
	if (whealth_bar_utility)
	{

		MyHealthBar = CreateWidget<UUserWidget>(GetWorld(), whealth_bar_utility);
		/** Make sure widget was created */
		if (MyHealthBar)
		{
			/** Add it to the viewport */
			MyHealthBar->AddToViewport();
		}
	}
}

void AMyHUD::UpdateHealth(float value)
{
	if (MyHealthBar)
	{

	}
}

