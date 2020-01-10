// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthParentWidget.generated.h"

/**
 * 
 */
UCLASS()
class BRIANSAWESOMEGAME_API UHealthParentWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Value")
		float HealthPercentCPP;

	UFUNCTION(Category = "Gameplay")
		void UpdateUIHealth(float hp);
	
};
