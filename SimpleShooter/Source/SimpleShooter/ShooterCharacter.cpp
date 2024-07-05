// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterCharacter.h"
#include "Gun.h"
#include "Components/CapsuleComponent.h"
#include "SimpleShooterGameModeBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

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
	Health = MaxHealth;
	Gun = GetWorld()->SpawnActor<AGun>(GunClass);
	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	Gun->SetOwner(this);
	Camera = FindComponentByClass<UCameraComponent>();
	if(Camera){
		DefaultFieldOfView = Camera->FieldOfView;
	}
}

bool AShooterCharacter::IsDead() const{
	if(Health<=0){
		return true;
	}
	return false;
}

float AShooterCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(bIsZooming)
	{
		ZoomAim(DeltaTime);
	}
	else if(!bIsZooming && Camera->FieldOfView != DefaultFieldOfView){
		ResetZoom(DeltaTime);
	}
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AShooterCharacter::LookUpRate);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &AShooterCharacter::LookRightRate);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AShooterCharacter::Shoot);
	PlayerInputComponent->BindAction(TEXT("Zoom"), EInputEvent::IE_Pressed, this, &AShooterCharacter::SetZoomBool);
	PlayerInputComponent->BindAction(TEXT("Zoom"), EInputEvent::IE_Released, this, &AShooterCharacter::SetZoomBool);
	
}

float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if(DamageToApply>Health){
		DamageToApply = Health;
	}
	Health -= DamageToApply;
	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);
	if(IsDead()){
		ASimpleShooterGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASimpleShooterGameModeBase>();
		if(GameMode != nullptr){
			GameMode->PawnKilled(this);
		}
		DetachFromControllerPendingDestroy();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	return DamageToApply;

}
void AShooterCharacter::MoveForward(float AxisValue)
{
	if(AxisValue<ControllerDeadZone && AxisValue>-ControllerDeadZone){
		AxisValue=0;
	}
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

void AShooterCharacter::MoveRight(float AxisValue)
{
	if(AxisValue<ControllerDeadZone && AxisValue>-ControllerDeadZone){
		AxisValue=0;
	}
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void AShooterCharacter::LookUpRate(float AxisValue){
	if(AxisValue<ControllerDeadZone && AxisValue>-ControllerDeadZone){
		AxisValue=0;
	}
	AddControllerPitchInput(AxisValue*RotationRate * GetWorld()->GetDeltaSeconds());
}
void AShooterCharacter::LookRightRate(float AxisValue){
	if(AxisValue<ControllerDeadZone && AxisValue>-ControllerDeadZone){
		AxisValue=0;
	}
	AddControllerYawInput(AxisValue*RotationRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::Shoot(){
	Gun->PullTrigger();
}



void AShooterCharacter::ZoomAim(float DeltaTime){
	if(Camera){
		Camera->FieldOfView = FMath::FInterpTo(Camera->FieldOfView, ZoomFieldOfView, DeltaTime, ZoomSpeed);
	}
}

void AShooterCharacter::ResetZoom(float DeltaTime){
	bIsZooming = false;
	if(Camera){
		Camera->FieldOfView = FMath::FInterpTo(Camera->FieldOfView, DefaultFieldOfView, DeltaTime, ZoomSpeed);;
	}
}


void AShooterCharacter::SetZoomBool()
{
	bIsZooming = !bIsZooming;
}