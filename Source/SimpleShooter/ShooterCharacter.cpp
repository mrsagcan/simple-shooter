// Fill out your copyright notice in the Description page of Project Settings.


//TODO: Add Main Menu
	//Play button.
	//Settings button.
		//Sound setting
		//Graphics quality setting
		//Brightness setting
	//Quit button.

#include "ShooterCharacter.h"
#include "Gun.h"
#include "Engine/DamageEvents.h"
#include "Components/CapsuleComponent.h"
#include "SimpleShooterGameModeBase.h"


// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapons();

	Health = MaxHealth;

}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AShooterCharacter::LookUpRate);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &AShooterCharacter::LookRightRate);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AShooterCharacter::Shoot);
	PlayerInputComponent->BindAction(TEXT("NextWeapon"), EInputEvent::IE_Pressed, this, &AShooterCharacter::NextWeapon);
	PlayerInputComponent->BindAction(TEXT("PreviousWeapon"), EInputEvent::IE_Pressed, this, &AShooterCharacter::PreviousWeapon);
	PlayerInputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &AShooterCharacter::Reload);
}

float AShooterCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageApplied = FMath::Min(Health, DamageApplied);
	Health -= DamageApplied;
	UE_LOG(LogTemp, Display, TEXT("Health: %f"), Health);

	if (IsDead())
	{
		ASimpleShooterGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASimpleShooterGameModeBase>();
		if (GameMode != nullptr)
		{
			GameMode->PawnKilled(this);
		}

		DetachFromControllerPendingDestroy();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	return DamageApplied;
}

void AShooterCharacter::Shoot()
{
	Guns[ActiveIndex]->PullTrigger();
}

void AShooterCharacter::Reload()
{
	Guns[ActiveIndex]->Reload();
}

void AShooterCharacter::NextWeapon()
{
	ActivateWeapon(ActiveIndex + 1);
}

void AShooterCharacter::PreviousWeapon()
{
	ActivateWeapon(ActiveIndex - 1);
}

bool AShooterCharacter::IsDead() const
{
	return Health <= 0;
}

float AShooterCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

AGun* AShooterCharacter::GetEquippedWeapon() const
{
	return Guns[ActiveIndex];
}

void AShooterCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

void AShooterCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void AShooterCharacter::LookUpRate(float AxisValue)
{
	AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookRightRate(float AxisValue)
{
	AddControllerYawInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}



void AShooterCharacter::SpawnWeapons()
{
	for (auto GunClass : GunClasses)
	{
		AGun* Gun = GetWorld()->SpawnActor<AGun>(GunClass);
		GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
		Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
		Gun->SetOwner(this);
		Guns.Add(Gun);
	}
	ActivateWeapon(0);
}

void AShooterCharacter::ActivateWeapon(int32 Index)
{
	for (auto Weapon : Guns)
	{
		Weapon->SetActorHiddenInGame(true);
	}
	ActiveIndex = Index < 0 ? Index + Guns.Num() : Index % Guns.Num();
	Guns[ActiveIndex]->SetActorHiddenInGame(false);
}

