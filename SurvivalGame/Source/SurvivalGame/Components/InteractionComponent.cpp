// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InteractionComponent.h"

UInteractionComponent::UInteractionComponent()
{
	SetComponentTickEnabled(true);

	InteractionTime = 0.f;
	InteractionDistance = 200.f;
	InteractableNameText = FText::FromString("Interactable Object");
	InteractableActionText = FText::FromString("Interact");
	bAllowMultipleInteractors = true;

	//UI �ɼ� (WidgetComponent��)
	Space = EWidgetSpace::Screen;
	DrawSize = FIntPoint(400, 100); //UIũ��
	bDrawAtDesiredSize = true;

	SetActive(true);
	SetHiddenInGame(true); // �⺻������ UI�����
}