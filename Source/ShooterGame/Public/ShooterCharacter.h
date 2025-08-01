// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class SHOOTERGAME_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called for forward/backward
	void MoveForward(float Value);


	// left/right
	void MoveRight(float Value);

	/*
	Called via input to turn at given rata
	@param Rate This is a normalized rate ie 1.0 means 100% disired turn rate
	*/
	void TurnAtRate(float Rate);

	/*
	Called via input to look up or down at given rata
	@param Rate This is a normalized rate ie 1.0 means 100% disired look up rate
	*/
	void LookUpAtRate(float Rate);


	/**
	* Rotate controller based on mouse X movement
	* @param Value   The input value from mouse movement
	*/
	void Turn(float Value);

	/**
	* Rotate controller based on mouse Y movement
	* @param Value   The input value from mouse movement
	*/
	void LookUp(float Value);

	// called when the fire button is pressed
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& MuzzleSocektLocation, FVector& OutBeamLocation);

	void AimingButtonPressed();
	void AimingButtunReleased();

	// ineterpolate the zoom in and zoom out
	void CameraInterpZoom(float DeltaTime);

	// set base turn and lookup rate based on aming
	void SetLookRate();

	void CalcualateCorsshairSpread(float DeltaTime);

	void StartCrosshairBulletFire();

	UFUNCTION()
	void FinishCrosshairBulletFire();

	void FireButtonPressed();
	void FireButtonReleased();

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
	// Camera Boom positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	//Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// Base turn rate in deg/sec. other scaling may effect final turn rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	// Base look up/down in deg/sec. other scaling may effect final turn rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AmingTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AmingLookUpRate;


	// Scale factor mouse look sensitivity . Turn rate when not aming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = 0.0, UIMax = 1.0))
	float MouseHipTurnRate;


	// Scale factor mouse look sensitivity . lookup rate when not aming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = 0.0, UIMax = 1.0))
	float MouseHipLookUpRate;


	// Scale factor mouse look sensitivity . Turn rate when  aming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = 0.0, UIMax = 1.0))
	float MouseAmingTurnRate;


	// Scale factor mouse look sensitivity . lookup rate when  aming
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = 0.0, UIMax = 1.0))
	float MouseAmingLookUpRate;


	// Gun shot sound cue
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	// Flash spawned at Barrel Socket
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	// Montage for Firing the Weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	// Particle Spawned where the bullet hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticle;

	// Smoke Trail for bullet
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticle;

	// true when aming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	// default  camera FOV value
	float CameraDefaultFOV;

	// FOV when zoomed in
	float CameraZoomFOV;

	// current fOV at a given frame
	float CameraCurrentFOV;

	// Interp speed for zooming when aming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomItnterSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	float ShootTimeDuration;

	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;

	// left mouse or right consile trigger pressed
	bool bFireButtonPressed;

	// True when we can fire. Flase when waiting for the timer
	bool bShouldFire;

	// Rate of automatic gun fire
	float AutomaticFireRate;

	// Sets a timer between gun shots
	FTimerHandle AutoFireTimer;


public:
	// Return Camera boom
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// Return Camerea
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool GetAming() const { return bAiming; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;



};
