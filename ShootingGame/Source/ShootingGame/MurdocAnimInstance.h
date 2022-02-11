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
	// �������Ʈ���� �����ؼ� ������Ʈ(Tick) �Լ�ó�� �� �Լ�
	UFUNCTION(BlueprintCallable)
		void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		class AMurdoc* ShooterCharacter;

	// ĳ���� ���ǵ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float Speed;

	// ĳ���Ͱ� ���߿� �ִ���
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		bool bIsInAir;

	// ĳ���Ͱ� �����̰� �ִ���
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		bool bIsAccelerating;
};

