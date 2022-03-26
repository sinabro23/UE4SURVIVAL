// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InteractionComponent.h"
// Sets default values
ASurvivalCharacter::ASurvivalCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(GetMesh(), FName("CameraSocket")); // ī�޶� �Ӹ����󰡰� �ϱ����� ������ ���� �Ӹ� ���̷�Ż�� �����߾���
	CameraComponent->bUsePawnControlRotation = true; // ���콺 ���� ��Ʈ�ѷ��� �����̼��� �������

	HelmetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HelmetMesh"));
	HelmetMesh->SetupAttachment(GetMesh());
	HelmetMesh->SetMasterPoseComponent(GetMesh()); // �Ӹ��� �ִϸ��̼� �������Ʈ������ �����ߴ��� ��ġ������ �߰��߾���

	ChestMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ChestMesh"));
	ChestMesh->SetupAttachment(GetMesh());
	ChestMesh->SetMasterPoseComponent(GetMesh()); // �޽��� ��� ���󰡰� �ϱ�����

	LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegsMesh"));
	LegsMesh->SetupAttachment(GetMesh());
	LegsMesh->SetMasterPoseComponent(GetMesh());

	FeetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FeetMesh"));
	FeetMesh->SetupAttachment(GetMesh());
	FeetMesh->SetMasterPoseComponent(GetMesh());

	VestMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VestMesh"));
	VestMesh->SetupAttachment(GetMesh());
	VestMesh->SetMasterPoseComponent(GetMesh());

	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandsMesh"));
	HandsMesh->SetupAttachment(GetMesh());
	HandsMesh->SetMasterPoseComponent(GetMesh());

	BackpackMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BackpackMesh"));
	BackpackMesh->SetupAttachment(GetMesh());
	BackpackMesh->SetMasterPoseComponent(GetMesh());

	InteractionCheckFrequency = 0.f; // �� �����Ӹ��� ��ȣ�ۿ밡������ üũ
	InteractionCheckDistance = 1000.f;

	GetMesh()->SetOwnerNoSee(true); // �� ĳ������ ���ʴ� �޽��� ���� ����. �츮�� �޽��� �Ӹ��� �����߾���

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true; // ũ���ġ ��� �����ϰ�

}

// Called when the game starts or when spawned
void ASurvivalCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASurvivalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const bool bIsInteractinOnServer = (HasAuthority() && IsInteracting());

	// �������� ���� ����Ʈ���̽� ���ϰ� 
	if ((!HasAuthority() || bIsInteractinOnServer) && GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}

}

void ASurvivalCharacter::PerformInteractionCheck()
{
	if (!GetController()) // ��Ʈ�ѷ��� ������ ����
		return;

	FVector EyesLoc; // ī�޶� ��ġ
	FRotator EyesRot; // ī�޶� �����̼�

	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	GetController()->GetPlayerViewPoint(EyesLoc, EyesRot); // ĳ������ ������Ʈ�� �����̵� 

	// ī�޶�κ��� ����ĳ��Ʈ�� �� �� 
	FVector TraceStart = EyesLoc;
	FVector TraceEnd = (EyesRot.Vector() * InteractionCheckDistance) + TraceStart;

	FHitResult TraceHit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // ����ĳ��Ʈ �Ҷ� �� ĳ���ʹ� �����ϰ� �ϱ� ���� �Լ�
	
	if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		if (TraceHit.GetActor()) // ���� �¾Ҵٸ�
		{
			//���� ���Ͱ� UInteractionComopnent�� ��� �ִٸ� // TraceHit.GetActor()->GetComponentByClass(UInteractionComponent::StaticClass()) // �̰� �����ҵ�
			if (UInteractionComponent* InteractionComponent = Cast<UInteractionComponent>(TraceHit.GetActor()->GetComponentByClass(UInteractionComponent::StaticClass())))
			{
				float Distance = (TraceStart - TraceHit.ImpactPoint).Size(); // �������� ������������ �Ÿ�
				
				if (InteractionComponent != GetInteractable() && Distance <= InteractionComponent->InteractionDistance) // �츮�� ���� ���� Ʈ���̽��� ������Ʈ�� ������ �� ����ü�� �̹� �����س��� interactionComponent�� �ƴ϶��
				{
					FoundNewInteractable(InteractionComponent); 
				}
				else if(Distance > InteractionComponent->InteractionDistance && GetInteractable()) // ��ȣ�ۿ��� �� �ִ� �Ÿ��� �ʹ� �ָ�
				{
					CouldntFindInteractable();
				}

				return;
			}
		}
	}

	CouldntFindInteractable();
}

void ASurvivalCharacter::CouldntFindInteractable()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_Interact))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interact);
	}

	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->EndFocus(this);

		if (InteractionData.bInteractHeld)
		{
			EndInteract();
		}
	}

	InteractionData.ViewedInteractionComponent = nullptr;
}

void ASurvivalCharacter::FoundNewInteractable(UInteractionComponent* Interactable)
{
	EndInteract(); // ������ Ȱ��ȭ �Ȱ� ������ ������ ����

	if (UInteractionComponent* OldInteractable = GetInteractable()) // ���� ����ü�� �� �ִ� ��
	{
		OldInteractable->EndFocus(this);
	}

	InteractionData.ViewedInteractionComponent = Interactable;
	Interactable->BeginFocus(this); //
}



void ASurvivalCharacter::BeginInteract()
{
	if (!HasAuthority()) //HasAuthority �������� �ƴ��� ����
	{
		ServerBeginInteract();
	}

	InteractionData.bInteractHeld = true;

	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->BeginInteract(this);

		if (FMath::IsNearlyZero(Interactable->InteractionTime))
		{
			Interact();
		}
		else
		{
			GetWorldTimerManager().SetTimer(TimerHandle_Interact, this, &ASurvivalCharacter::Interact, Interactable->InteractionTime, false);
		}
	}
}

void ASurvivalCharacter::EndInteract()
{
	if (!HasAuthority()) //HasAuthority �������� �ƴ��� ����
	{
		ServerEndInteract();
	}

	InteractionData.bInteractHeld = false;

	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->EndInteract(this);
	}
}

void ASurvivalCharacter::ServerBeginInteract_Implementation()
{
	BeginInteract();
}

bool ASurvivalCharacter::ServerBeginInteract_Validate()
{
	return true;
}


void ASurvivalCharacter::ServerEndInteract_Implementation()
{
	EndInteract();
}

bool ASurvivalCharacter::ServerEndInteract_Validate()
{
	return true;
}

void ASurvivalCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->Interact(this);
	}
}

bool ASurvivalCharacter::IsInteracting() const
{
	return GetWorldTimerManager().IsTimerActive(TimerHandle_Interact);
}

float ASurvivalCharacter::GetRemainingInteractTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(TimerHandle_Interact);
}

void ASurvivalCharacter::MoveForward(float Val)
{
	if (Val != 0)
	{
		AddMovementInput(GetActorForwardVector(), Val);
	}
}

void ASurvivalCharacter::MoveRight(float Val)
{
	if (Val != 0)
	{
		AddMovementInput(GetActorRightVector(), Val);
	}
}

void ASurvivalCharacter::LookUp(float Val)
{
	if (Val != 0)
	{
		AddControllerPitchInput(Val);
	}

}

void ASurvivalCharacter::Turn(float Val)
{
	if (Val != 0)
	{
		AddControllerYawInput(Val);
	}
}

void ASurvivalCharacter::StartCrouching()
{
	Crouch();
}

void ASurvivalCharacter::StopCrouching()
{
	UnCrouch();
}


// Called to bind functionality to input
void ASurvivalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Courch", EInputEvent::IE_Pressed, this, &ASurvivalCharacter::StartCrouching);
	PlayerInputComponent->BindAction("Courch", EInputEvent::IE_Released, this, &ASurvivalCharacter::StopCrouching);

	PlayerInputComponent->BindAction("Interact", EInputEvent::IE_Pressed, this, &ASurvivalCharacter::BeginInteract);
	PlayerInputComponent->BindAction("Interact", EInputEvent::IE_Released, this, &ASurvivalCharacter::EndInteract);


	PlayerInputComponent->BindAxis("MoveForward", this, &ASurvivalCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASurvivalCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ASurvivalCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &ASurvivalCharacter::Turn);
}

