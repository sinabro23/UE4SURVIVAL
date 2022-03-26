// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SurvivalCharacter.generated.h"

USTRUCT()
struct FInteractionData
{
	GENERATED_BODY()

	FInteractionData()
	{
		ViewedInteractionComponent = nullptr;
		LastInteractionCheckTime = 0.f;
		bInteractHeld = false;
	}

	UPROPERTY()
	class UInteractionComponent* ViewedInteractionComponent; // ���� �츮�� ���Ѻ����ִ� InteractionComponent

	UPROPERTY()
	float LastInteractionCheckTime; // The time when we last Checked for an Interactable

	UPROPERTY()
	bool bInteractHeld; // whether the local player is holding the interact key
};


UCLASS()
class SURVIVALGAME_API ASurvivalCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASurvivalCharacter();

	UPROPERTY(EditAnywhere, Category = "Components")
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	USkeletalMeshComponent* HelmetMesh;

	UPROPERTY(EditAnywhere, Category = "Components")
	USkeletalMeshComponent* ChestMesh;

	UPROPERTY(EditAnywhere, Category = "Components")
	USkeletalMeshComponent* LegsMesh;

	UPROPERTY(EditAnywhere, Category = "Components")
	USkeletalMeshComponent* FeetMesh;

	UPROPERTY(EditAnywhere, Category = "Components")
	USkeletalMeshComponent* VestMesh;

	UPROPERTY(EditAnywhere, Category = "Components")
	USkeletalMeshComponent* HandsMesh;

	UPROPERTY(EditAnywhere, Category = "Components")
	USkeletalMeshComponent* BackpackMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//��ȣ�ۿ� ����
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCheckFrequency; // ��ȣ�ۿ� ������ ������Ʈ�� ��� �ֱ�� üũ����. set this to zero if you want to check everyframe

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCheckDistance; // How far we'll trace when we check if the player is looking at an interactable object

	void PerformInteractionCheck();

	void CouldntFindInteractable(); // PerformInteractionCheck���� �����ϸ� ����� �Լ�
	void FoundNewInteractable(UInteractionComponent* Interactable); // PerformInteractionCheck���� �����ϸ� ����� �Լ�

	void BeginInteract();
	void EndInteract();

	void Interact();

	UPROPERTY()
	FInteractionData InteractionData; // ��ȣ�ۿ���õ� ����ü

	FORCEINLINE UInteractionComponent* GetInteractable() const { return InteractionData.ViewedInteractionComponent; }
	///////////////

	void MoveForward(float Val);
	void MoveRight(float Val);

	void LookUp(float Val);
	void Turn(float Val);

	void StartCrouching();
	void StopCrouching();
public:	


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
