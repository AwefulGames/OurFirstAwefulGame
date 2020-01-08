// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#include "MyHUD.h"
#include "BriansAwesomeGameGameMode.h"
#include "BriansAwesomeGameCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"

ABriansAwesomeGameGameMode::ABriansAwesomeGameGameMode() : Super()
{

	// use our custom HUD class
	HUDClass = AMyHUD::StaticClass();


	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ABriansAwesomeGameGameMode::BeginPlay()
{
	Super::BeginPlay();
	FString deathMessage = FString::Printf(TEXT("GAME MODE SUPER PLAY"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, deathMessage);

}