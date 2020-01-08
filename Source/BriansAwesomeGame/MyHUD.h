// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Components/TimelineComponent.h"
#include "Components/BoxComponent.h"
#include "MyHUD.generated.h"

UCLASS()
class BRIANSAWESOMEGAME_API AMyHUD : public AHUD
{
	GENERATED_BODY()
public:
	AMyHUD();
	// Primary draw call for the HUD.
	//virtual void DrawHUD() override;

	virtual void BeginPlay() override;
	//virtual void Tick(float DeltaSeconds) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float FullHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float HealthPercentage;

	// Reference UMG Asset in the Editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UUserWidget> whealth_bar_utility;

	// Variable to hold the widget After Creating it.
	UUserWidget* MyHealthBar;

	UFUNCTION(Category = "Gameplay")
		void UpdateHealth(float value);

		
};
