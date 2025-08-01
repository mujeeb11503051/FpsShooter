// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	if (ShooterCharacter)
	{
		// Get lateral speed of Character from velocity
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0;
		speed = Velocity.Size();

		// is the Character in Air
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		// is Character Accelarating
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelarating = true;
		}
		else
		{
			bIsAccelarating = false;
		}

		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		//FString RotationMessage = FString::Printf(TEXT("Base Aim Rotation: %f"), AimRotation.Yaw);

		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
		//FString MovementRotationMessage = FString::Printf(TEXT("Movement Rotation: %f"), MovementRotation.Yaw);

		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
		//FString OffsetMessage = FString::Printf(TEXT("Offset Yaw: %f"), MovementOffsetYaw);

		if (ShooterCharacter->GetVelocity().Size() > 0)
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}

		bAming = ShooterCharacter->GetAming();


		if (GEngine)
		{
			//GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, OffsetMessage);
		}
	}
}

// Like begin Play() for Animation
void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}