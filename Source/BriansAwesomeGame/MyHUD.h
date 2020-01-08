// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Components/TimelineComponent.h"
#include "Components/BoxComponent.h"
#include "HealthParentWidget.h"
#include "MyHUD.generated.h"

UCLASS()
class BRIANSAWESOMEGAME_API AMyHUD : public AHUD
{
	GENERATED_BODY()
public:
	AMyHUD();
	// Primary draw call for the HUD.
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;
	//virtual void Tick(float DeltaSeconds) override;


	// Reference UMG Asset in the Editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UHealthParentWidget> wBestHealthBar; 

	// Variable to hold the widget After Creating it.
	UHealthParentWidget* MyHealthBar;

	UFUNCTION(Category = "Gameplay")
		void UpdateHUD(float hp) {
		if (MyHealthBar) MyHealthBar->UpdateUIHealth(hp);
	}


		
};
