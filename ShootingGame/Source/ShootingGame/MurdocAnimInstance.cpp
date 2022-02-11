// Fill out your copyright notice in the Description page of Project Settings.


#include "MurdocAnimInstance.h"
#include "Murdoc.h"
#include "GameFramework/CharacterMovementComponent.h"

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
		Velocity.Z = 0; // �����ϰų� �������ų� �Ҷ��� �ӵ��� ���� �ʱ� ���� 0���� ����
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
	}
}

void UMurdocAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AMurdoc>(TryGetPawnOwner());

}

