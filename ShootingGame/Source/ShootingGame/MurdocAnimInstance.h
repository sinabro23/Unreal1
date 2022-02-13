// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MurdocAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGGAME_API UMurdocAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	// 블루프린트에서 연결해서 업데이트(Tick) 함수처럼 쓸 함수
	UFUNCTION(BlueprintCallable)
		void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		class AMurdoc* ShooterCharacter;

	// 캐릭터 스피드
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float Speed;

	// 캐릭터가 공중에 있는지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		bool bIsInAir;

	// 캐릭터가 움직이고 있는지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		bool bIsAccelerating;

	// 와리가리 하기 위한 Yaw offset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	// stop애니메이션에 쓸 오프셋을 기억하고 있기위해 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bAiming;
};

