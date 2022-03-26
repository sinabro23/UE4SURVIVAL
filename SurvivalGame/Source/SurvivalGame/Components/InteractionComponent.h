// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InteractionComponent.generated.h"

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

};
