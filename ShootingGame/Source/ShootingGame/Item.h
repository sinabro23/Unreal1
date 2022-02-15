// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM(BlueprintType) // 블루프린트 베이스로한 타입 만들 수 있게 해줌
enum class EItemRarity : uint8
{
	EIR_Damaged UMETA(DisplayName = "Damaged"), // 블루프린트에서 만들떄 EIR빠지고 이 이름으로 나옴
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_UnCommon UMETA(DisplayName = "UnCommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),

	EIR_Max UMETA(DisplayName = "DefaultMax")
};


UCLASS()
class SHOOTINGGAME_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// AddDynamic에 들어가는 함수는 인자 맞춰줘야함
	// areasphere에 오버랩 되면 호출
	UFUNCTION()
		void OnSphereOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	// areasphere에 오버랩이 끝나면
	UFUNCTION()
		void OnSphereEndOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	// ActiveStarts array of bools 설정해줌
	void SetActiveStars();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// 아이템 메쉬(스켈레탈메쉬임)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;

	// 라인트레이스가 박스와 충돌하면 HUD띄워주게 하기 위해
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;

	// 아이템 쳐다보면 팝업되는 위젯
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* PickupWidget;

	// 오버랩됐을떼 아이템 트레이싱 가능하게
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AreaSphere;

	// 아이템 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FString ItemName;

	// 아이템 개수(총알 개수 등등)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 ItemCount;

	// 아이템 레어도 // 위젯에서 별 개수 정해줌
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemRarity ItemRarity;

	// 5개 별마다 가질 불값 ( 참이면 별표시 아니면 표시안함 )
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TArray<bool> ActiveStars;

public:
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; };

};
