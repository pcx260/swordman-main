// Copyright Epic Games, Inc. All Rights Reserved.

#include "locomotionsystemCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include"DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include"Kismet/KismetMathLibrary.h"
#include"enemy_cpp.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AlocomotionsystemCharacter

AlocomotionsystemCharacter::AlocomotionsystemCharacter()
{
	
	

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate
	
	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	

	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AlocomotionsystemCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//variables
	animidle1 = 0;
	bblock = false;
	isattack = false;
	ldamage = false;
	health = 100.0f;
	attackover = false;
	line = false;
	animplay = false;
	
	
}



//////////////////////////////////////////////////////////////////////////
// Input

void AlocomotionsystemCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AlocomotionsystemCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AlocomotionsystemCharacter::Look);
		//sprint
		EnhancedInputComponent->BindAction(sprintAction, ETriggerEvent::Triggered, this, &AlocomotionsystemCharacter::sprint);
		EnhancedInputComponent->BindAction(sprintAction, ETriggerEvent::Completed, this, &AlocomotionsystemCharacter::unsprint);

		//block
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &AlocomotionsystemCharacter::block);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &AlocomotionsystemCharacter::unblock);
		//attack
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AlocomotionsystemCharacter::attack);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AlocomotionsystemCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		//
		//GetComponentRotation();
		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

		
		
	}
}

void AlocomotionsystemCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AlocomotionsystemCharacter::sprint()
{

	if (bblock==false)
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
	
}

void AlocomotionsystemCharacter::block()
{
	bblock = true;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	
}

void AlocomotionsystemCharacter::unblock()
{
	bblock = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

}

void AlocomotionsystemCharacter::unsprint()
{
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}



void AlocomotionsystemCharacter::ftrace()
{
	FHitResult hitresult;
	
	FCollisionQueryParams querryparam;
	querryparam.AddIgnoredActor(this);
	
	FVector start = GetMesh()->GetSocketLocation("start");
	FVector end = GetMesh()->GetSocketLocation("end");
	
	FVector finish = (GetActorForwardVector() * 1000) + GetActorLocation();

	
	if (line==true)
	{
				GetWorld()->LineTraceSingleByChannel(hitresult, start, end, ECollisionChannel::ECC_Camera, querryparam);
				DrawDebugLine(GetWorld(), start, end, FColor::Blue, false, 5.0f);
	}

	
	
	if (hitresult.GetActor())
	{
		
		
		Aenemy_cpp* enemy = Cast<Aenemy_cpp>(hitresult.GetActor());

		if (enemy)
		{

			if (attackover==false)
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("ffff"));
				enemy->takedamage = true;
				attackover = true;
				enemy->health -= 20;
				
			
			}
			
		}

			
			
				

			
			
			
			
		

	}
}

void AlocomotionsystemCharacter::attack()
{
	if (isattack==false)
	{
		isattack = true;
		animplay = true;
	}



}

void AlocomotionsystemCharacter::target()
{

	//AlocomotionsystemCharacter UGameplayStatics::GetActorOfClass(GetWorld(), TSubclassOf<Aenemy_cpp>());
	

	Aenemy_cpp* enemy = Cast<Aenemy_cpp>(UGameplayStatics::GetActorOfClass(GetWorld(), Aenemy_cpp::StaticClass()));
	//FVector start = GetActorLocation();
	FVector start = GetActorLocation();
	FVector end = enemy->GetActorLocation();
	float yaw = UKismetMathLibrary::FindLookAtRotation(start, end).Yaw;

	
	
	if (bblock==true)
	{
		GetController()->SetControlRotation(FRotator(0, yaw, 0));
		
	}
	else if ()
	{
		SetActorRotation(FRotator(0, yaw, 0));
	}
		

}

void AlocomotionsystemCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	

	ftrace();

	
	speed = GetVelocity().Size();
	
	target();
	
	//SetActorRotation(actorlocal);

	if (speed > 100, bblock == true)
	{

		
		
		SetActorRotation(FRotator(0, GetControlRotation().Yaw, 0));
		
		

	}

}