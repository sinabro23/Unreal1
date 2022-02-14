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

	// 마우스로 턴과 룩업관리
	void Turn(float Value);

	void LookUp(float Value);

	// called via input to look up/down at a given rate.
	// @param rate	this is a normalized rate, i.e means 100% of desired rate
	void LookUpAtRate(float Rate);

	void FireWeapon();

	// 총쏘면 빔의 끝의 로케이션을 구하는 함수
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	// bAiming 을 true or false 로 설정
	void AimingButtonPressed();
	void AimingButtonReleased();

	void CameraInterpZoom(float DeltaTime);

	void SetLookRates();

	void CalculateCrosshairSpread(float DeltaTime);

	void FireButtonPressed();
	void FireButtonReleased();

	void StartFireTimer();
	UFUNCTION()
	void AutoFireReset();

	// 크로스헤어로 라인트레이싱해서 충돌되는 아이템 찾기
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

	// OverlappedItemCount>0이면 아이템 추적(정보창)
	void TraceForItems();

	void StartCrosshairBulletFire();

	UFUNCTION()
	void FinishCrosshairBulletFire();

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
	
	// 조준 안할때의 도는 비율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float HipTurnRate;

	// 조준 안할때 올려다보는 비율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float HipLookUpRate;

	// 조준할때
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float AimingTurnRate;

	// 조준할때
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float AimingLookUpRate;

	// 조준안할떄 마우스 감도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMx = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float MouseHipTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMx = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float MouseHipLookUpRate;

	// 조준 할 때 마우스 감도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMx = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float MouseAimingTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMx = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float MouseAimingLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;
	// flash spawnd at barrelsocket
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;
	// Montage for firing weapon.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	// 벽에 맞았을때의 fx
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	// 스모크 트레일 위한것.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	// true when aiming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		bool bAiming;

	// default camera FieldOfView 
	float CameraDefaultFOV;

	// FieldOfView when zoomed
	float CameraZoomedFOV;

	// 천천히 줌과 줌아닌상태를 바꾸게 하기위해 현재 FOV를 체크
	float CameraCurrentFOV;

	// 줌 바꿀때에 바뀌는 속도.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		float ZoomInterpSpeed;

	// 크로스 헤어 벌어짐을 정함
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshiars, meta = (AllowPrivateAccess = "true"))
		float CrosshairSpreadMultiplier;

	// 퍼지는 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshiars, meta = (AllowPrivateAccess = "true"))
		float CrosshairVelocityFactor;

	// 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshiars, meta = (AllowPrivateAccess = "true"))
		float CrosshairInAirFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshiars, meta = (AllowPrivateAccess = "true"))
		float CrosshairAimFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshiars, meta = (AllowPrivateAccess = "true"))
		float CrosshairShootingFactor;

	// 자동 사격을 위해 좌클릭했는지 체크
	bool bFireButtonPressed;

	// 타이머로 설정한 일정 시간 지나면 false
	bool bShouldFire;

	// 총알 나가는 간격
	float AutomaticFireRate;

	// 자동사격을 위한 타이머
	FTimerHandle AutoFireTimer;



	float ShootTimeDuration;

	bool bFiringBullet;

	FTimerHandle CrosshairShootTimer;

	// 매프레임마다 아이템을 trace해야하면
	bool bShouldTraceForItems;

	// 오버랩된 AItem의 개수
	int8 OverlappedItemCount;

public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool GetAiming() const { return bAiming; }

	UFUNCTION(BlueprintCallable)
		float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	// 추적하는 아이템이 0이되면 bShouldTraceForItems를 false로 만듦
	void IncrementOverlappedItemCount(int8 Amount);
};
