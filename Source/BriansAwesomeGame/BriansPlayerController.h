// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HealthParentWidget.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BriansPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BRIANSAWESOMEGAME_API ABriansPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
		ABriansPlayerController(const FObjectInitializer& ObjectInitializer);

		virtual void BeginPlay() override;
	// Reference UMG Asset in the Editor
	//TSubclassOf<class UHealthParentWidget> wBestHealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UHealthParentWidget> wBestHealthBar;

	// The instance of the players Inventory UI Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player, HUD and UI")
		class UHealthParentWidget* MyHealthBar;

	UFUNCTION(Category = "Gameplay")
		void UpdateHealth(float hpf);

	
};
