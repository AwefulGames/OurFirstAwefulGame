// Fill out your copyright notice in the Description page of Project Settings.


#include "BriansPlayerController.h"
#include "Engine/Engine.h"
#include "HealthParentWidget.h"
#include "UObject/ConstructorHelpers.h"


ABriansPlayerController::ABriansPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	ConstructorHelpers::FClassFinder<UHealthParentWidget> UIClassFinder(TEXT("WidgetBlueprint'/Game/UI_Widgets/BestHealthBar.BestHealthBar_C'"));
	wBestHealthBar = UIClassFinder.Class;
}

void ABriansPlayerController::BeginPlay() {
	Super::BeginPlay();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("1")));
	if (wBestHealthBar) // Check the selected UI class is not NULL
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("2")));
		if (!MyHealthBar) // If the widget is not created and == NULL
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("3")));
			MyHealthBar = CreateWidget<UHealthParentWidget>(GetWorld(), wBestHealthBar); // Create Widget
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("3.5")));
			if (!MyHealthBar)
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("4!")));
			//return;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("5")));
			MyHealthBar->AddToViewport(); // Add it to the viewport so the Construct() method in the UUserWidget:: is run.
			MyHealthBar->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ABriansPlayerController::UpdateHealth(float hpf) {
	if (MyHealthBar) MyHealthBar->UpdateUIHealth(hpf);
}