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

	//UI 옵션 (WidgetComponent임)
	Space = EWidgetSpace::Screen;
	DrawSize = FIntPoint(400, 100); //UI크기
	bDrawAtDesiredSize = true;

	SetActive(true);
	SetHiddenInGame(true); // 기본적으로 UI숨길것
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
	const bool bPlayerAlreadyInteracting = !bAllowMultipleInteractors && Interactors.Num() >= 1; // 1이상이면 동시에 2명이상 상호작용 중이니깐
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

// 캐릭터가 쳐다보면
void UInteractionComponent::BeginFocus(ASurvivalCharacter* Character)
{
	if (!IsActive() || !GetOwner() || !Character)
		return;

	OnBeginFocus.Broadcast(Character);

	SetHiddenInGame(false); // UI보이게 하기 위해

	if (!GetOwner()->HasAuthority()) //서버가 아니면
	{
		TArray<UActorComponent*> Components;

		GetOwner()->GetComponents(UPrimitiveComponent::StaticClass(), Components);

		for (auto VisualComp : Components)
		{
			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
			{
				UE_LOG(LogTemp, Warning, TEXT("BeginFocus"));
				Prim->SetRenderCustomDepth(true); // 윤곽 설정한다는데 아직 잘 모르겠음
			}
		}
	}
}

void UInteractionComponent::EndFocus(ASurvivalCharacter* Character)
{
	OnEndFocus.Broadcast(Character);

	SetHiddenInGame(true); // UI안 보이게 하기 위해

	if (!GetOwner()->HasAuthority()) //서버가 아니면
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
