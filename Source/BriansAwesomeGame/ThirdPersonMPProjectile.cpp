// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonMPProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"


#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Engine/Engine.h"





// Sets default values
AThirdPersonMPProjectile::AThirdPersonMPProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	//Definition for the SphereComponent that will serve as the Root component for the projectile and its collision.
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SphereComponent->InitSphereRadius(10.0f);
	SphereComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = SphereComponent;

	//Registering the Projectile Impact function on a Hit event.
	if (Role == ROLE_Authority)
	{
		SphereComponent->OnComponentHit.AddDynamic(this, &AThirdPersonMPProjectile::OnProjectileImpact);
	}

	//Definition for the Mesh that will serve as our visual representation.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMesh->SetupAttachment(RootComponent);

	//Set the Static Mesh and its position/scale if we successfully found a mesh asset to use.
	if (DefaultMesh.Succeeded())
	{
		StaticMesh->SetStaticMesh(DefaultMesh.Object);
		StaticMesh->RelativeLocation = FVector(0.0f, 0.0f, -11.0f);
		StaticMesh->RelativeScale3D = FVector(10.0f/37.5f, 10.0f / 37.5f, 10.0f / 37.5f);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (DefaultExplosionEffect.Succeeded())
	{
		ExplosionEffect = DefaultExplosionEffect.Object;
	}

	//Definition for the Projectile Movement Component.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);
	ProjectileMovementComponent->InitialSpeed = 1000.0f;
	ProjectileMovementComponent->MaxSpeed = 1000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	DamageType = UDamageType::StaticClass();
	Damage = 34.0f;


	// load the sound cue object
	static ConstructorHelpers::FObjectFinder<USoundCue> ImpactSoundCueObject(TEXT("SoundCue'/Game/StarterContent/Audio/Explosion_Cue.Explosion_Cue'"));
	if (ImpactSoundCueObject.Succeeded())
	{
		ImpactSoundCue = ImpactSoundCueObject.Object;

		ImpactAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ImpactAudioComponent"));
		ImpactAudioComponent->SetupAttachment(RootComponent);
	}

}

// Called when the game starts or when spawned
void AThirdPersonMPProjectile::BeginPlay()
{
	Super::BeginPlay();

	// make sure our audio variables are initialized
	if (ImpactSoundCue && ImpactAudioComponent) {
		// attach the sound cue to our audio component 
		// NOTE: do not do this in the constructor as it will play the sound when the player spawns
		ImpactAudioComponent->SetSound(ImpactSoundCue);
	}


}

// Called every frame
void AThirdPersonMPProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AThirdPersonMPProjectile::Destroyed()
{
	

}

void AThirdPersonMPProjectile::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		//if (OtherActor == GetWorld()->GetFirstPlayerController()->GetPawn()) {
		//	return;
		//}

		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, NormalImpulse, Hit, Instigator->Controller, this, DamageType);
	}

	FVector spawnLocation = GetActorLocation() - GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();;
	ImpactAudioComponent->SetRelativeLocation(spawnLocation);

	// check to make sure the audio component is initialized and we are not already playing a sound
	if (ImpactAudioComponent && !ImpactAudioComponent->IsPlaying())
	{
		//ImpactAudioComponent->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));

		// activate the sound if it has not been already
		if (!ImpactAudioComponent->IsActive())
		{
			ImpactAudioComponent->Activate(true);
		}
		// default pitch value 1.0f
		// modify the pitch to create variance by grabbing a random float between 1.0 and 1.3
		//ImpactAudioComponent->SetPitchMultiplier(FMath::RandRange(1.0f, 1.3f));
		// play the sound
		ImpactAudioComponent->Play(0.f);
		ImpactAudioComponent->OnAudioFinished.AddDynamic(this, &AThirdPersonMPProjectile::OnAudioFinished);
	}

	// Hides visible components
	SetActorHiddenInGame(true);

	// Disables collision components
	SetActorEnableCollision(false);

	spawnLocation = GetActorLocation();
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, spawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
}

void AThirdPersonMPProjectile::OnAudioFinished() {
	Destroy();
}