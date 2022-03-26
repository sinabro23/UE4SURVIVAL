// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InteractionComponent.generated.h"


// ��������Ʈ�� �������Ʈ������ �Լ��� �����ϰ� �� �� ���� 9�� ���� 25�еڿ� Ȯ��
// ���ڷ� ĳ���� �޾Ҵµ� �Լ��� ĳ���͸� ���� �� ����.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginInteract, class ASurvivalCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndInteract, class ASurvivalCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginFocus, class ASurvivalCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndFocus, class ASurvivalCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, class ASurvivalCharacter*, Character);

/**
 *  ��ȣ�ۿ��� �ϰ��ϱ����� ������Ʈ
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent)) // �������Ʈ������ ������Ʈ ���� �����ϰ�
class SURVIVALGAME_API UInteractionComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionTime; // ��ȣ�ۿ��� �ϴµ� �ɸ��� �ð�

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionDistance; // ��ȣ�ۿ��� �Ҽ��ִ� �����Ÿ�

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractableNameText; // ��ȣ�ۿ��� ���� �̸� (ex) Lamp

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractableActionText; // � ��ȣ�ۿ��� �Ұ��� // ex) turn off

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bAllowMultipleInteractors; // �������� ���ÿ� ���ͷ��� �Ҽ� �ִ���

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractableNameText(const FText& NewNameText);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractableActionText(const FText& ActionText);

	//Delegates

	//[local + server] Called when the player presses the interact key whilst focusing on this interactable actor
	// BlueprintAssignable �������Ʈ���� �Լ� �Ҵ� ����. ex) ���� �� �Ѱ� ����⸦ �������Ʈ �̺�Ʈ �׷����� ����
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginInteract OnBeginInteract;

	//[local + server] Called when the player releases the interact key, stops looking at the interactable actor, or gets too far away after starting an interact
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnEndInteract OnEndInteract;

	//[local + server] Called when the player presses the interact key whilst focusing on this interactable actor
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginFocus OnBeginFocus;

	//[local + server] Called when the player releases the interact key, stops looking at the interactable actor, or gets too far away after starting an interact
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnEndFocus OnEndFocus;

	//[local + server] Called when the player has interacted with the item for the required amount of time
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginInteract OnInteract;

protected:

	virtual void Deactivate() override;

	bool CanInteract(class ASurvivalCharacter* Character) const;

	TArray<class ASurvivalCharacter*> Interactors;

public:

	void RefreshWidget();

	//Called on the client when the players interaction check trace begin/ends hitting this item (�Ĵٺ���������)
	void BeginFocus(class ASurvivalCharacter* Character);
	void EndFocus(class ASurvivalCharacter* Character);

	//Called on the client when the player begins/ends interaction with the item(eŰ ��������)
	void BeginInteract(class ASurvivalCharacter* Character);
	void EndInteract(class ASurvivalCharacter* Character);

	// ��¥ ���ͷ����Ҷ�
	void Interact(class ASurvivalCharacter* Character);

	//Return a value from 0-1 denoting how far through the interact we are.
	//On Server this is the first interactors percentage, on client this is the local interactors percentage
	UFUNCTION(BlueprintPure, Category = "Interaction")
	float GetInteractPercentage();
};
