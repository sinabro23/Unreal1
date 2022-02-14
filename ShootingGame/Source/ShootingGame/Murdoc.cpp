// Fill out your copyright notice in the Description page of Project Settings.


#include "Murdoc.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Item.h"
#include "Components/WidgetComponent.h"

// Sets default values
AMurdoc::AMurdoc() :
	// �⺻ ���� ����
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	// ���̹� �������� ���¼ӵ� ������ ������ �ϴ� ������
	HipTurnRate(90.f),
	HipLookUpRate(90.f),
	AimingTurnRate(20.f),
	AimingLookUpRate(20.f),
	// ���콺 ���� ���� ����
	MouseHipTurnRate(1.f),
	MouseHipLookUpRate(1.f),
	MouseAimingTurnRate(0.4f),
	MouseAimingLookUpRate(0.4f),
	// �����ϴ����� ���� �Ұ�
	bAiming(false),
	// ī�޶� FOV ������
	CameraDefaultFOV(0.f), // set in Beginplay
	CameraZoomedFOV(35.f),
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(20.f),
	// ũ�ν���� �������� ��ҵ�
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f),
	// �ݹ� Ÿ�̸� ������
	ShootTimeDuration(0.05f),
	bFiringBullet(false),
	// �ڵ���� ���� ũ�ν���� �������� �ð����ٴ� ũ���ؾ���(0.5f����) �ȱ׷��� ��ӹ�����
	AutomaticFireRate(0.1f),
	bShouldFire(true),
	bFireButtonPressed(false),
	// ������ ���� ������
	bShouldTraceForItems(false),
	OverlappedItemCount(0)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the character if there is a colliison)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f; // the camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // attach camera to end of boom
	FollowCamera->bUsePawnControlRotation = false; // camera does not rotate relative to arm

	// Don't rotate when the controller rotates. let the controller only affect the camera.
	// ���콺 ������ ĳ���� �ȵ����.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // character moves in the derection of input.
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.f));
}

// Called when the game starts or when spawned
void AMurdoc::BeginPlay()
{
	Super::BeginPlay();

	//�̶��� ī�޶� ���� �Ǿ������ϱ� ���� fov���� �����ϴ°� ����	
	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

}

void AMurdoc::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void AMurdoc::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		// ����?�� ���õȰ� ������ �� �𸣰ڴ�.
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
}

void AMurdoc::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMurdoc::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds()); // deg/sec * sec/frame
}

void AMurdoc::Turn(float Value)
{
	float TurnScaleFactor{};

	if (bAiming)
	{
		TurnScaleFactor = MouseAimingTurnRate;
	}
	else
	{
		TurnScaleFactor = MouseHipTurnRate;
	}
	AddControllerYawInput(Value * TurnScaleFactor);
}

void AMurdoc::LookUp(float Value)
{
	float LookUpScaleFactor{};

	if (bAiming)
	{
		LookUpScaleFactor = MouseAimingLookUpRate;
	}
	else
	{
		LookUpScaleFactor = MouseHipLookUpRate;
	}
	AddControllerPitchInput(Value * LookUpScaleFactor);
}

void AMurdoc::FireWeapon()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

		if (MuzzleFlash) // �ѱ� �÷���
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(
			SocketTransform.GetLocation(),
			BeamEnd);

		if (bBeamEnd)
		{
			if (ImpactParticles) // �´��ʿ� ȿ��
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					BeamEnd);
			}

			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				BeamParticles,
				SocketTransform);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}

	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}

	// �ݹ߽� ũ�ν���� ���� Ÿ�̸� ����
	StartCrosshairBulletFire();
}

bool AMurdoc::GetBeamEndLocation(
	const FVector& MuzzleSocketLocation,
	FVector& OutBeamLocation)
{
	// ũ�ν���� Ʈ���̽� üũ
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

	if (bCrosshairHit) // ũ�ν����� ���� �¾�����
	{
		// �Ͻ����� beam location - ���� �ѱ��κ��� üũ ������
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else // ũ�ν����� ���� �ȸ¾�����
	{
		// �̹� TraceUnderCrosshairs�Լ����� OutBeamLocation���� ����Ʈ���̽� ���������� ������
	}

	// �ѱ��κ��� �ݸ��� üũ(ũ�ν����κ����ϸ� �տ� �浹������ �ȵ���)
	FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBeamLocation - MuzzleSocketLocation };
	const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f };
	GetWorld()->LineTraceSingleByChannel(
		WeaponTraceHit,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility);
	if (WeaponTraceHit.bBlockingHit) // �ѱ��κ��� ����Ʈ���̽��ؼ� �浹 �ƴٸ�
	{
		OutBeamLocation = WeaponTraceHit.Location;
		return true;
	}

	return false;
}

void AMurdoc::AimingButtonPressed()
{
	bAiming = true;
}

void AMurdoc::AimingButtonReleased()
{
	bAiming = false;
}

void AMurdoc::CameraInterpZoom(float DeltaTime)
{
	// ���� ī�޶��� fov�� ���� 
	if (bAiming)
	{
		// ZoomInterpSpeed �ӵ� ��ŭ ����� õõ�� �ٲ�
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraZoomedFOV, // ��ǥ��
			DeltaTime,
			ZoomInterpSpeed);
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraDefaultFOV, // ��ǥ��
			DeltaTime,
			ZoomInterpSpeed);
	}

	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}

void AMurdoc::SetLookRates()
{
	if (bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

void AMurdoc::CalculateCrosshairSpread(float DeltaTime)
{
	// �̵��ӵ� ������ ����.
	FVector2D WalkSpeedRange{ 0.f, 600.f };
	// �ӵ� �ۼ�Ʈ ���� 0~1
	FVector2D VelocityMultiplierRange{ 0.f,1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f; // ���� �پ �̵��ϴ°Ÿ� �Ǵ��ؾ��ϴϱ�

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
		WalkSpeedRange,
		VelocityMultiplierRange,
		Velocity.Size());

	// CrosshairInAirFactor���ϱ� ���� (�����Ҷ� ũ�ν���� ������ ����)
	if (GetCharacterMovement()->IsFalling()) // ĳ���Ͱ� ���߿� ������
	{
		CrosshairInAirFactor = FMath::FInterpTo( // õõ�� �� �ٲٱ�
			CrosshairInAirFactor,
			2.25f, // CrosshairInAirFactor�� ��ǥ�� �ϴ� ��
			DeltaTime,
			2.25f); // CrosshairInAirFactor�� ��ǥ���� ���� �ӵ�
	}
	else // ĳ���Ͱ� ������ ������
	{
		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor,
			0.f,
			DeltaTime,
			30.f); // ���� ��������� ������ ũ�ν���� ���� ���������ϴϱ�
	}

	if (bAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(
			CrosshairAimFactor,
			0.6f,
			DeltaTime,
			30.f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(
			CrosshairAimFactor,
			0.f,
			DeltaTime,
			30.f);
	}

	// 0.05�ʵ��� Ʈ����
	if (bFiringBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(
			CrosshairShootingFactor,
			0.3f,
			DeltaTime,
			60.f
		);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(
			CrosshairShootingFactor,
			0.f,
			DeltaTime,
			60.f
		);
	}

	CrosshairSpreadMultiplier =
		0.5f +
		CrosshairVelocityFactor +
		CrosshairInAirFactor -
		CrosshairAimFactor +
		CrosshairShootingFactor;
}

void AMurdoc::FireButtonPressed()
{
	bFireButtonPressed = true;
	StartFireTimer();
}

void AMurdoc::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AMurdoc::StartFireTimer()
{
	if (bShouldFire)
	{
		FireWeapon();
		bShouldFire = false;
		GetWorldTimerManager().SetTimer(
			AutoFireTimer,
			this,
			&AMurdoc::AutoFireReset,
			AutomaticFireRate);
	}
}

void AMurdoc::AutoFireReset()
{
	bShouldFire = true;
	if (bFireButtonPressed)
	{
		StartFireTimer();
	}
}

bool AMurdoc::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	// ũ�ν���� ��ġ ã������ ����Ʈ ������ ���ϱ�
	FVector2D ViewportSize; // ���� ����Ʈ ũ�� ���ϱ�
	if (GEngine && GEngine->GameViewport)
	{
		// ����Ʈ ����� ViewportSize�� ����
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f); // ũ�ν���� ��ġ�� ����Ʈ�� �����̴ϱ�
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// �̰��ϰ��� ũ�ν����� ī�޶� �ݴ������ ���⺤�Ϳ� ũ�ν������ ������������ ����
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		// ũ�ν���� ��������̼����κ���
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * 50'000.f };
		OutHitLocation = End; // ������ ������ OutHitLocation�� �׳� ����Ʈ���̽� �ִ�Ÿ�

		GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);

		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}
	return false;
}

void AMurdoc::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceUnderCrosshairs(ItemTraceResult, HitLocation);
		if (ItemTraceResult.bBlockingHit)
		{
			AItem* HitItem = Cast<AItem>(ItemTraceResult.Actor);
			if (HitItem && HitItem->GetPickupWidget())
			{
				// ������ ������ �������.
				HitItem->GetPickupWidget()->SetVisibility(true);
			}
		}
	}
}

void AMurdoc::StartCrosshairBulletFire()
{
	bFiringBullet = true;

	// �ѽ�� 5�ʵ��� bFiringBullet�� true����
	GetWorldTimerManager().SetTimer(
		CrosshairShootTimer,
		this,
		&AMurdoc::FinishCrosshairBulletFire,
		ShootTimeDuration);
}

void AMurdoc::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

// Called every frame
void AMurdoc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraInterpZoom(DeltaTime); // ī�޶� FOV õõ�� �ٲ��
	// ���콺 ���� ����
	SetLookRates();
	// ũ�ν���� ������ ���
	CalculateCrosshairSpread(DeltaTime);

	// �������� ������ ������ üũ�ϰ� �׷��� �����Ѵ�
	TraceForItems();

}

// Called to bind functionality to input
void AMurdoc::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMurdoc::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMurdoc::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("TurnRate"), this, &AMurdoc::TurnAtRate);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AMurdoc::LookUpAtRate);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AMurdoc::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMurdoc::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this,
		&AMurdoc::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this,
		&AMurdoc::FireButtonReleased);

	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this,
		&AMurdoc::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this,
		&AMurdoc::AimingButtonReleased);
}

float AMurdoc::GetCrosshairSpreadMultiplier() const
{

	return CrosshairSpreadMultiplier;
}

void AMurdoc::IncrementOverlappedItemCount(int8 Amount)
{
	if ((OverlappedItemCount + Amount) <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

