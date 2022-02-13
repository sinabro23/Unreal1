// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Murdoc.generated.h"

UCLASS()
class SHOOTINGGAME_API AMurdoc : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMurdoc();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// called for fowards/backwards input
	void MoveForward(float Value);

	// called for side to side input
	void MoveRight(float Value);

	// Called via input  to turn at a given rate.
	// @param rate this is a normarlized rate, i.e. 1.0 means 100% of desired turn rate
	void TurnAtRate(float Rate);

	// called via input to look up/down at a given rate.
	// @param rate	this is a normalized rate, i.e means 100% of desired rate
	void LookUpAtRate(float Rate);

	void FireWeapon();

	// �ѽ�� ���� ���� �����̼��� ���ϴ� �Լ�
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	// bAiming �� true or false �� ����
	void AimingButtonPressed();
	void AimingButtonReleased();

	void CameraInterpZoom(float DeltaTime);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	// base turn rate, in deg/sec. other scaling may affect final turn rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	// base look up/down rate, in deg/sec other scaling may affect final turn rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;
	// flash spawnd at barrelsocket
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;
	// Montage for firing weapon.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	// ���� �¾������� fx
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	// ����ũ Ʈ���� ���Ѱ�.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	// true when aiming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		bool bAiming;

	// default camera FieldOfView 
	float CameraDefaultFOV;

	// FieldOfView when zoomed
	float CameraZoomedFOV;

	// õõ�� �ܰ� �ܾƴѻ��¸� �ٲٰ� �ϱ����� ���� FOV�� üũ
	float CameraCurrentFOV;

	// �� �ٲܶ��� �ٲ�� �ӵ�.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float ZoomInterpSpeed;

public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool GetAiming() const { return bAiming; }
};
