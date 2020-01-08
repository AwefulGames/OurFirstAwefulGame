// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BriansAwesomeGameCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/GameModeBase.h"


#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

#include "Kismet/GameplayStatics.h"


#include "ThirdPersonMPProjectile.h"

//////////////////////////////////////////////////////////////////////////
// ABriansAwesomeGameCharacter

ABriansAwesomeGameCharacter::ABriansAwesomeGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)


	//Initialize the player's Health
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

	//Initialize projectile class
	ProjectileClass = AThirdPersonMPProjectile::StaticClass();
	//Initialize fire rate
	FireRate = 0.25f;
	bIsFiringWeapon = false;

	bReplicates = true;

}


void ABriansAwesomeGameCharacter::RestartCharacter()
{
	// Set size for collision capsule
	//GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	//FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)


	//Initialize the player's Health
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

	//Initialize projectile class
	//ProjectileClass = AThirdPersonMPProjectile::StaticClass();
	//Initialize fire rate
	FireRate = 0.25f;
	bIsFiringWeapon = false;

	bReplicates = true;


	character_rotator = FRotator(0.0f, 0.0f, 0.0f);
}


//////////////////////////////////////////////////////////////////////////
// Input

void ABriansAwesomeGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABriansAwesomeGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABriansAwesomeGameCharacter::MoveRight);

	PlayerInputComponent->BindAction("RotateDudeCW", IE_Pressed, this, &ABriansAwesomeGameCharacter::RollCameraAndPawnCW);
	PlayerInputComponent->BindAction("RotateDudeCCW", IE_Pressed, this, &ABriansAwesomeGameCharacter::RollCameraAndPawnCCW);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABriansAwesomeGameCharacter::CameraControlYaw);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABriansAwesomeGameCharacter::CameraControlPitch);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ABriansAwesomeGameCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ABriansAwesomeGameCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ABriansAwesomeGameCharacter::OnResetVR);

	// Handle firing projectiles
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABriansAwesomeGameCharacter::StartFire);
}


//////////////////////////////////////////////////////////////////////////
// Replicated Properties

void ABriansAwesomeGameCharacter::GetLifetimeReplicatedProps(TArray <FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate current health.
	DOREPLIFETIME(ABriansAwesomeGameCharacter, CurrentHealth);
}


void ABriansAwesomeGameCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ABriansAwesomeGameCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ABriansAwesomeGameCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ABriansAwesomeGameCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABriansAwesomeGameCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ABriansAwesomeGameCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		Direction = character_rotator.RotateVector(Direction);
		AddMovementInput(Direction, Value);
	}
}

void ABriansAwesomeGameCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction

		Direction = character_rotator.RotateVector(Direction);
		AddMovementInput(Direction, Value);
	}
}

void ABriansAwesomeGameCharacter::OnHealthUpdate()
{
	//Client-specific functionality
	if (IsLocallyControlled())
	{
		FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

		if (CurrentHealth <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
			PlayerDied();
			respawn = true;
		}
	}

	//Server-specific functionality
	if (Role == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}

	//Functions that occur on all machines. 
	/*
		Any special functionality that should occur as a result of damage or death should be placed here.
	*/
}

void ABriansAwesomeGameCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}


void ABriansAwesomeGameCharacter::SetCurrentHealth(float healthValue)
{
	if (Role == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
		OnHealthUpdate();
	}
}

float ABriansAwesomeGameCharacter::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{

	DamageVector = DamageCauser->GetActorRightVector();
	DamageLocation = DamageCauser->GetActorLocation();
	float damageApplied = CurrentHealth - DamageTaken;
	SetCurrentHealth(damageApplied);
	return damageApplied;
}

void ABriansAwesomeGameCharacter::StartFire()
{
	if (!bIsFiringWeapon)
	{
		bIsFiringWeapon = true;
		UWorld* World = GetWorld();
		World->GetTimerManager().SetTimer(FiringTimer, this, &ABriansAwesomeGameCharacter::StopFire, FireRate, false);
		HandleFire();
	}
}

void ABriansAwesomeGameCharacter::StopFire()
{
	bIsFiringWeapon = false;
}

void ABriansAwesomeGameCharacter::HandleFire_Implementation()
{
	FVector spawnLocation = GetActorLocation() + (GetControlRotation().Vector()  * 100.0f) + (GetActorUpVector() * 50.0f);
	FRotator spawnRotation = GetControlRotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = Instigator;
	spawnParameters.Owner = this;

	AThirdPersonMPProjectile* spawnedProjectile = GetWorld()->SpawnActor<AThirdPersonMPProjectile>(spawnLocation, spawnRotation, spawnParameters);
}

void ABriansAwesomeGameCharacter::PlayerDied() {
	// Make sure everybody else replicates this stuff
	PassDeadInformation(this, DamageLocation);
}


void ABriansAwesomeGameCharacter::ReplicateDeadPlayer_Implementation(ABriansAwesomeGameCharacter * DeadGuy, FVector ImpactLocation) {

	FString deathMessage = FString::Printf(TEXT("Client Function Called"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, deathMessage);

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABriansAwesomeGameCharacter::StaticClass(), FoundActors);

	//for (AActor* TActor : FoundActors)
	//{
		//ABriansAwesomeGameCharacter* MyActor = Cast<ABriansAwesomeGameCharacter>(TActor);

		if (DeadGuy != nullptr) {

			deathMessage = FString::Printf(TEXT("Actor"));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, deathMessage);
			// Do what ever you need with your MyActor

			USkeletalMeshComponent* Mesh3P = DeadGuy->GetMesh();

			/* Disable all collision on capsule */
			UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
			CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

			Mesh3P->SetCollisionProfileName(TEXT("Ragdoll"));
			//SetActorEnableCollision(true);


			// Ragdoll
			Mesh3P->SetAllBodiesSimulatePhysics(true);
			Mesh3P->SetSimulatePhysics(true);
			Mesh3P->WakeAllRigidBodies();
			Mesh3P->bBlendPhysics = true;
			Mesh3P->SetIsReplicated(true);
			UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
			if (CharacterComp)
			{
				CharacterComp->StopMovementImmediately();
				CharacterComp->DisableMovement();
				CharacterComp->SetComponentTickEnabled(false);
				CharacterComp->SetIsReplicated(true);
			}



			/* Apply physics impulse on the bone of the enemy skeleton mesh we hit (ray-trace damage only) */

			Mesh3P->AddRadialImpulse(ImpactLocation, 150, 2000, ERadialImpulseFalloff::RIF_Linear, true);
		//}
	}

}

void ABriansAwesomeGameCharacter::PassDeadInformation_Implementation(ABriansAwesomeGameCharacter * DeadGuy, FVector ImpactLocation) {

	FString deathMessage = FString::Printf(TEXT("Server Function Called"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, deathMessage);

	ReplicateDeadPlayer(DeadGuy, ImpactLocation);
	
}

// Called every frame
void ABriansAwesomeGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector OldAccel = GetCharacterMovement()->GetCurrentAcceleration();

	FString healthMessage = FString::Printf(TEXT("OldA %f %f %f -- Forcing %f %f %f"), OldAccel.X, OldAccel.Y, OldAccel.Z, CharacterAccel.X, CharacterAccel.Y, CharacterAccel.Z);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, healthMessage);


	LaunchVelocity = CharacterAccel * DeltaTime  + GetCharacterMovement()->GetLastUpdateVelocity();
	ABriansAwesomeGameCharacter::LaunchCharacter(LaunchVelocity, true, true);

	if (respawn) {
		respawn_counter+=DeltaTime;

		if (respawn_counter > 5.0) {
			respawn = false;

			// Make a location for the new actor to spawn at  this actor
			FVector NewLocation = GetActorLocation() + FVector(0.f, 0.f, 0.f);

			RespawnOurGuy(this->GetController(), NewLocation);

			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Respawning")));


		}
	}

}


void ABriansAwesomeGameCharacter::RespawnOurGuy_Implementation(AController * controller_for_guy, FVector NewLocation) {
	
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Server Respawning")));

	// Spawn the new actor (Using GetClass() instead of AMySpawner so that if someone derives a new class  
	// from AMySpawner we spawn an instance of that class instead)  
	ABriansAwesomeGameCharacter* NewActor = GetWorld()->SpawnActor<ABriansAwesomeGameCharacter>(GetClass(), NewLocation, FRotator::ZeroRotator);

	// save a copy of our controller
	AController* SavedController = controller_for_guy;
	// unpossess first ( helps with multiplayer )
	SavedController->UnPossess();
	// disable movement mode
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	// possess our new actor
	SavedController->Possess(Cast<ABriansAwesomeGameCharacter>(NewActor));
}
	
void ABriansAwesomeGameCharacter::RollCameraAndPawn(float rotation_amount) {
	character_rotator += FRotator(0, 0, rotation_amount);

	FollowCamera->RelativeRotation = character_rotator;

	FRotator NewRotation = GetActorRotation() + FRotator(0, 0, rotation_amount);

	SetActorRotation(NewRotation);

	float mass = 88.8; // Kg
	float accel = 1; // g

	FVector NewAccel = FVector(0, 0, accel);

	CharacterAccel = character_rotator.RotateVector(NewAccel);

}

void ABriansAwesomeGameCharacter::RollCameraAndPawnCW() {
	RollCameraAndPawn(90.0f);
}

void ABriansAwesomeGameCharacter::RollCameraAndPawnCCW() {
	RollCameraAndPawn(-90.0f);
}

void ABriansAwesomeGameCharacter::CameraControlYaw(float Rate)
{
	// calculate delta for this frame from the rate information
	FVector  BaseRate(0, 0, Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());

	FVector NewRate;

	NewRate = character_rotator.RotateVector(BaseRate);

	AddControllerRollInput(NewRate.X);
	AddControllerPitchInput(NewRate.Y);
	AddControllerYawInput(NewRate.Z);
}

void ABriansAwesomeGameCharacter::CameraControlPitch(float Rate)
{
	// calculate delta for this frame from the rate information
	FVector  BaseRate(0, Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds(), 0);

	FVector NewRate;

	NewRate = character_rotator.RotateVector(BaseRate);

	AddControllerRollInput(NewRate.X);
	AddControllerPitchInput(NewRate.Y);
	AddControllerYawInput(NewRate.Z);
}