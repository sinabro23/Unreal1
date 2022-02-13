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

// Sets default values
AMurdoc::AMurdoc():
	// 기본 도는 비율
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	// 에이밍 기준으로 도는속도 빠르게 느리게 하는 변수들
	HipTurnRate(90.f),
	HipLookUpRate(90.f),
	AimingTurnRate(20.f),
	AimingLookUpRate(20.f),
	// 마우스 감도 관련 변수
	MouseHipTurnRate(1.f),
	MouseHipLookUpRate(1.f),
	MouseAimingTurnRate(0.4f),
	MouseAimingLookUpRate(0.4f),
	// 조준하는지에 대한 불값
	bAiming(false),
	// 카메라 FOV 변수들
	CameraDefaultFOV(0.f), // set in Beginplay
	CameraZoomedFOV(35.f),
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(20.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the character if there is a colliison)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 220.f; // the camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0.0f, 60.f, 70.f);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // attach camera to end of boom
	FollowCamera->bUsePawnControlRotation = false; // camera does not rotate relative to arm

	// Don't rotate when the controller rotates. let the controller only affect the camera.
	// 마우스 돌려도 캐릭터 안따라옴.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // character moves in the derection of input.
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Murdoc(TEXT("SkeletalMesh'/Game/ParagonMurdock/Characters/Heroes/Murdock/Meshes/Murdock.Murdock'"));
	if (SM_Murdoc.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM_Murdoc.Object);
	}
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.f));
}

// Called when the game starts or when spawned
void AMurdoc::BeginPlay()
{
	Super::BeginPlay();
	//이때는 카메라가 생성 되어있으니깐 지금 fov관련 설정하는게 맞음	
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
		// 외적?과 관련된것 같은데 잘 모르겠다.
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

void AMurdoc::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds()); // deg/sec * sec/frame
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

		if (MuzzleFlash) // 총구 플레임
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(
			SocketTransform.GetLocation(),
			BeamEnd);

		if (bBeamEnd)
		{
			if (ImpactParticles) // 맞는쪽에 효과
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
}

bool AMurdoc::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{
	//크로스헤어 포지션에서 Linetrace 만들려한다
	FVector2D ViewportSize; // 현재 뷰포트 크기 구하기
	if (GEngine && GEngine->GameViewport)
	{
		// 뷰포트 사이즈가 ViewportSize에 나옴
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	CrosshairLocation.Y -= 50.f; // HUD블루프린트에서 Y올렸었음
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// 이걸하고나면 크로스헤어에서 카메라 반대방향의 방향벡터와 크로스헤어의 월드포지션이 나옴
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld) // 투영이 성공적이었다면
	{
		FHitResult ScreenTraceHit;
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ CrosshairWorldPosition + CrosshairWorldDirection * 50'000.f };

		OutBeamLocation = End;	 // 아무것도 맞지 않으면 라인트레이스의 엔드포인트가 빔의 엔드가될것

		GetWorld()->LineTraceSingleByChannel( // 크로스헤어로부터 라인트레이스함
			ScreenTraceHit,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);

		// 크로스헤어로부터 맞았다면
		if (ScreenTraceHit.bBlockingHit)
		{
			OutBeamLocation = ScreenTraceHit.Location;

		}

		// 총구로부터 콜리전 체크(크로스헤어로부터하면 앞에 충돌판정이 안됐음)
		FHitResult WeaponTraceHit;
		const FVector WeaponTraceStart{ MuzzleSocketLocation };
		const FVector WeaponTraceEnd{ OutBeamLocation };
		GetWorld()->LineTraceSingleByChannel(
			WeaponTraceHit,
			WeaponTraceStart,
			WeaponTraceEnd,
			ECollisionChannel::ECC_Visibility);
		if (WeaponTraceHit.bBlockingHit) // 총구로부터 라인트레이스해서 충돌 됐다면
		{
			OutBeamLocation = WeaponTraceHit.Location;
		}

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
	// 현재 카메라의 fov를 설정 
	if (bAiming)
	{
		// ZoomInterpSpeed 속도 만큼 밸류가 천천히 바뀜
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraZoomedFOV, // 목표값
			DeltaTime,
			ZoomInterpSpeed);
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraDefaultFOV, // 목표값
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
	// 이동속도 범위를 설정.
	FVector2D WalkSpeedRange{ 0.f, 600.f };
	// 속도 퍼센트 단위 0~1
	FVector2D VelocityMultiplierRange{ 0.f,1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f; // 땅에 붙어서 이동하는거만 판단해야하니깐

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
		WalkSpeedRange,
		VelocityMultiplierRange,
		Velocity.Size());

	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor;
}

// Called every frame
void AMurdoc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraInterpZoom(DeltaTime);
	SetLookRates();
	CalculateCrosshairSpread(DeltaTime);
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
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AMurdoc::FireWeapon);

	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this,
		&AMurdoc::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this,
		&AMurdoc::AimingButtonReleased);
}

float AMurdoc::GetCrosshairSpreadMultiplier() const
{

	return CrosshairSpreadMultiplier;
}
