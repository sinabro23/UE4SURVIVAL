// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InteractionComponent.h"
#include "Player/SurvivalCharacter.h"
#include "Widgets/InteractionWidget.h"

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


void UInteractionComponent::SetInteractableNameText(const FText& NewNameText)
{
	InteractableNameText = NewNameText;
	RefreshWidget();
}

void UInteractionComponent::SetInteractableActionText(const FText& ActionText)
{
	InteractableActionText = ActionText;
	RefreshWidget();
}

void UInteractionComponent::Deactivate()
{
	Super::Deactivate();

	for (int i = Interactors.Num() - 1; i >= 0; --i)
	{
		if (ASurvivalCharacter* Interactor = Interactors[i])
		{
			EndFocus(Interactor);
			EndInteract(Interactor);
		}
	}

	Interactors.Empty();
}

bool UInteractionComponent::CanInteract(ASurvivalCharacter* Character) const
{
	const bool bPlayerAlreadyInteracting = !bAllowMultipleInteractors && Interactors.Num() >= 1; // 1�̻��̸� ���ÿ� 2���̻� ��ȣ�ۿ� ���̴ϱ�
	return !bPlayerAlreadyInteracting && IsActive() && GetOwner() != nullptr && Character != nullptr;
}

void UInteractionComponent::RefreshWidget()
{
	if (!bHiddenInGame && GetOwner()->GetNetMode() != NM_DedicatedServer)
	{
		if (UInteractionWidget* InteractionWidget = Cast<UInteractionWidget>(GetUserWidgetObject()))
		{
			InteractionWidget->UpdateInteractionWidget(this);
		}
	}
}

// ĳ���Ͱ� �Ĵٺ���
void UInteractionComponent::BeginFocus(ASurvivalCharacter* Character)
{
	if (!IsActive() || !GetOwner() || !Character)
		return;

	OnBeginFocus.Broadcast(Character);

	SetHiddenInGame(false); // UI���̰� �ϱ� ����

	if (!GetOwner()->HasAuthority()) //������ �ƴϸ�
	{
		TArray<UActorComponent*> Components;

		GetOwner()->GetComponents(UPrimitiveComponent::StaticClass(), Components);

		for (auto VisualComp : Components)
		{
			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
			{
				UE_LOG(LogTemp, Warning, TEXT("BeginFocus"));
				Prim->SetRenderCustomDepth(true); // ���� �����Ѵٴµ� ���� �� �𸣰���
			}
		}
	}
}

void UInteractionComponent::EndFocus(ASurvivalCharacter* Character)
{
	OnEndFocus.Broadcast(Character);

	SetHiddenInGame(true); // UI�� ���̰� �ϱ� ����

	if (!GetOwner()->HasAuthority()) //������ �ƴϸ�
	{
		TArray<UActorComponent*> Components;

		GetOwner()->GetComponents(UPrimitiveComponent::StaticClass(), Components);

		for (auto VisualComp : Components)
		{
			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
			{
				Prim->SetRenderCustomDepth(false);
			}
		}
	}
}

void UInteractionComponent::BeginInteract(ASurvivalCharacter* Character)
{
	if (CanInteract(Character))
	{
		Interactors.AddUnique(Character);
		OnBeginInteract.Broadcast(Character);
	}
}

void UInteractionComponent::EndInteract(ASurvivalCharacter* Character)
{
	Interactors.RemoveSingle(Character);
	OnEndInteract.Broadcast(Character);
}

void UInteractionComponent::Interact(ASurvivalCharacter* Character)
{
	if (CanInteract(Character))
	{
		OnInteract.Broadcast(Character);
	}
}

float UInteractionComponent::GetInteractPercentage()
{
	if (Interactors.IsValidIndex(0))
	{
		if (ASurvivalCharacter* Interactor = Interactors[0])
		{
			if (Interactor && Interactor->IsInteracting())
			{
				return 1.f - FMath::Abs(Interactor->GetRemainingInteractTime() / InteractionTime);
			}
		}
	}

	return 0.f;
}
