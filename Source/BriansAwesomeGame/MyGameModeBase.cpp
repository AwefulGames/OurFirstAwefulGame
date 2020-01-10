// Fill out your copyright notice in the Description page of Project Settings.

#include "MyHUD.h"
#include "MyGameModeBase.h"

AMyGameModeBase::AMyGameModeBase() {
	// use our custom HUD class
	HUDClass = AMyHUD::StaticClass();
}