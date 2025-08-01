// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter():
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),

	FireSound(nullptr),
	MuzzleFlash(nullptr),
	HipFireMontage(nullptr),
	BeamParticle(nullptr),
	ImpactParticle(nullptr),

	CameraZoomFOV(35.f),
	CameraDefaultFOV(0.f), // set this in begin play
	CameraCurrentFOV(0.f),
	ZoomItnterSpeed(20.f),

	HipTurnRate(90.f),
	HipLookUpRate(90.f),
	AmingTurnRate(20.f),
	AmingLookUpRate(20.f),

	MouseHipTurnRate(1.f),
	MouseHipLookUpRate(1.f),
	MouseAmingTurnRate(0.2f),
	MouseAmingLookUpRate(0.2f),

	// Crosshair spread factors
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f),

	ShootTimeDuration(0.05f),
	bFiringBullet(false),

	// auto fire variable
	AutomaticFireRate(.1f),
	bShouldFire(true),
	bFireButtonPressed(false),

	bAiming(false) // True when aming Weapon

{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Camera Boom 
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f; // camerea follow distance from the player
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based  on the controller
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach camera to end of boom
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Don't rotate when the controller rotates. Let the controller only affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;


	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ... at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
	
}

void AShooterCharacter::MoveForward(float Value) 
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		//findout which way is forward
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		//findout which way is forward
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}

}

void AShooterCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds()); // deg/sec * sec/frame
}

void AShooterCharacter::LookUpAtRate(float Rate)
{
	//UE_LOG(LogTemp, Warning, TEXT("LookUpAtRate called: %f"), Rate);
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds()); // deg/sec * sec/frame
}

void AShooterCharacter::Turn(float Value)
{
	float TurnScaleFactor{};
	if (bAiming)
	{
		TurnScaleFactor = MouseAmingTurnRate;
	}
	else
	{
		TurnScaleFactor = MouseHipTurnRate;
	}

	AddControllerYawInput(Value * TurnScaleFactor);
}

void AShooterCharacter::LookUp(float Value)
{
	float LookUpScaleFactor{};
	if (bAiming)
	{
		LookUpScaleFactor = MouseAmingLookUpRate;
	}
	else
	{
		LookUpScaleFactor = MouseHipLookUpRate;
	}

	AddControllerPitchInput(Value * LookUpScaleFactor);
}

void AShooterCharacter::FireWeapon()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);

		if (bBeamEnd)
		{
			// Spawn after updating beam end point either the end point from corsshair or gunbarrel
			if (ImpactParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, BeamEnd);
			}


			if (BeamParticle)
			{
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticle, SocketTransform);

				if (Beam)
				{
					Beam->SetVectorParameter(FName("Target"), BeamEnd);
				}
			}
		}


		//FHitResult FireHit;
		//const FVector Start{ SocketTransform.GetLocation() };
		//const FQuat Rotation{ SocketTransform.GetRotation() };
		//const FVector RotationAxis{ Rotation.GetAxisX() };
		//const FVector End{ Start + RotationAxis * 50'000.f };
		//FVector BeamEndPoint{ End };
		//GetWorld()->LineTraceSingleByChannel(FireHit, Start, End, ECollisionChannel::ECC_Visibility);
		//if (FireHit.bBlockingHit)
		//{
		//	/*DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f);
		//	DrawDebugPoint(GetWorld(), FireHit.Location, 5.f, FColor::Red, false, 4.f);*/
		//	BeamEndPoint = FireHit.Location;
		//	if (ImpactParticle)
		//	{
		//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, FireHit.Location);
		//	}
		//}
		//if (BeamParticle)
		//{
		//	UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticle, SocketTransform);
		//	if (Beam)
		//	{
		//		Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
		//	}
		//}
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
	// Start bullet fire timer for crosshairs
	StartCrosshairBulletFire();
}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocektLocation, FVector& OutBeamLocation)
{

	// Get current size of the viewport
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);

	}

	// Get screen space location of crosshairs
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	//CrosshairLocation.Y -= 50.f;

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// Get  world position and direction of crosshairs
	bool bScreenToWolrd = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (bScreenToWolrd) // was deprojction successful
	{
		FHitResult ScreenTraceHit;
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ CrosshairWorldPosition + CrosshairWorldDirection * 50'000 };

		// Set Beam end point to line trace
		OutBeamLocation = End;

		// Trace outward from corsshairs wolrd location
		GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, Start, End, ECollisionChannel::ECC_Visibility);

		// was there a trace hit
		if (ScreenTraceHit.bBlockingHit)
		{
			// beam endpoint now trace hit location
			OutBeamLocation = ScreenTraceHit.Location;
		}

		// Perfrom Second trace from the gun barrel
		FHitResult WeaponTraceHit;
		const FVector WeaponTraceStart{ MuzzleSocektLocation };
		const FVector WeaponTraceEnd{ OutBeamLocation };

		GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);

		if (WeaponTraceHit.bBlockingHit) // object between the barrel and BeamEndPoint
		{
			OutBeamLocation = WeaponTraceHit.Location;
		}

		return true;
	}
	return false;
}

// Set bAiming true/false
void AShooterCharacter::AimingButtonPressed()
{
	bAiming = true;
}

void AShooterCharacter::AimingButtunReleased()
{
	bAiming = false;
}

// Hanlde Interpolation for zoom
void AShooterCharacter::CameraInterpZoom(float DeltaTime)
{
	// set current fov
	if (bAiming)
	{
		// interpolate to zoom fov
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomFOV, DeltaTime, ZoomItnterSpeed);
	}
	else
	{
		// interpolate to default fov
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomItnterSpeed);
	}
	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}

void AShooterCharacter::SetLookRate()
{

	if (bAiming)
	{
		BaseTurnRate = AmingTurnRate;
		BaseLookUpRate = AmingLookUpRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

void AShooterCharacter::CalcualateCorsshairSpread(float DeltaTime)
{
	// calcualte crosshair velocity factor
	FVector2D WalkSpeedRange{ 0.f, 600.f };
	FVector2D VelocityMultiplierRange{ 0.f, 1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	// calculate crosshari Air Factor
	if (GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else // Character on the groud
	{
		// shrink crosshair fast
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	// calcualte Aim Factor
	if (bAiming)
	{
		// Shrink corss hairs small amount very quickly
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.6f, DeltaTime, 30.f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}

	// true 0.05 second after firing
	if (bFiringBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, .3f, DeltaTime, 60.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 60.f);
	}

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;
}

void AShooterCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;

	GetWorldTimerManager().SetTimer(CrosshairShootTimer, this, &AShooterCharacter::FinishCrosshairBulletFire,ShootTimeDuration);
}

void AShooterCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

void AShooterCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	StartFireTimer();
}

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
	if (bShouldFire)
	{
		FireWeapon();
		bShouldFire = false;
		GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AShooterCharacter::AutoFireReset, AutomaticFireRate);
	}
}

void AShooterCharacter::AutoFireReset() 
{
	bShouldFire = true;
	if (bFireButtonPressed)
	{
		StartFireTimer();
	}
}


// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CameraInterpZoom(DeltaTime);
	SetLookRate();
	CalcualateCorsshairSpread(DeltaTime);
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtunReleased);


}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{

	return CrosshairSpreadMultiplier;
}

