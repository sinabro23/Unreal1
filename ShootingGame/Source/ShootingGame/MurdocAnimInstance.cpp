// Fill out your copyright notice in the Description page of Project Settings.


#include "MurdocAnimInstance.h"
#include "Murdoc.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UMurdocAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (nullptr == ShooterCharacter)
	{
		ShooterCharacter = Cast<AMurdoc>(TryGetPawnOwner());
	}

	if (ShooterCharacter)
	{
		// Get the lateral speed of the character from velocity
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0; // 점프하거나 떨어지거나 할때의 속도는 넣지 않기 위해 0으로 설정
		Speed = Velocity.Size();

		// Is the Character in the air?
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		// Is The Character accelerating?
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		// 쳐다보고 있는 방향
		FRotator AimRotation =
			ShooterCharacter->GetBaseAimRotation();

		FRotator MovementRotation =
			UKismetMathLibrary::MakeRotFromX(
				ShooterCharacter->GetVelocity());

		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(
			MovementRotation,
			AimRotation).Yaw;

		//FString RotationMessage = FString::Printf(TEXT("Base Aim Rotation : %f"), AimRotation.Yaw);

		// 움직이는 방향 //#include "Kismet/KismetMathLibrary.h"

		//FString MovementRotationMessage = FString::Printf(TEXT("Movement Rotation : %f"), MovementRotation.Yaw);

	/*	FString OffsetMessage = FString::Printf(TEXT("Movement Offset Yaw : %f"), MovementOffsetYaw);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, OffsetMessage);
		}*/
	}
}

void UMurdocAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AMurdoc>(TryGetPawnOwner());

}

