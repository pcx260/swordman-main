// Fill out your copyright notice in the Description page of Project Settings.


#include "enemy_cpp.h"
#include"DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include"Kismet/GameplayStatics.h"
#include "locomotionsystemCharacter.h"



// Sets default values
Aenemy_cpp::Aenemy_cpp()
{
	health = 100.0f;
	alarm = false;
	isattack = false;
	takedamage = false;
	canattack = false;
	candamage = false;
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void Aenemy_cpp::trace()
{
	FHitResult hitresult;
	FVector start=GetMesh()->GetSocketLocation("start");
	FVector end = GetMesh()->GetSocketLocation("end");
	FCollisionQueryParams querryparams;
	querryparams.AddIgnoredActor(this);
	//AlocomotionsystemCharacter* locoactor = Cast<AlocomotionsystemCharacter>(hitresult.GetActor());
	
	FRotator aa;
	
	
	if (isattack == true)
	{
		GetWorld()->LineTraceSingleByChannel(hitresult, start, end, ECollisionChannel::ECC_Camera, querryparams);
		DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 5.0f);
	}
	
	
	if (hitresult.GetActor())
	{
		AlocomotionsystemCharacter* locoactor = Cast<AlocomotionsystemCharacter>(hitresult.GetActor());
		
		if (locoactor)
		{

			if (candamage==true)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("damage"));
				candamage = false;
				locoactor->ldamage = true;
		
				locoactor->health -= 20;
			
				

				
				//locoactor->ldamage = false;
				
			}
			
			
		}
		
	}
	
	if (alarm == true)
	{
		FHitResult detect;
		FVector tend = (GetActorForwardVector() * 100) + GetActorLocation();
		GetWorld()->LineTraceSingleByChannel(detect, GetActorLocation(), tend, ECollisionChannel::ECC_Camera, querryparams);
		AlocomotionsystemCharacter* locoactor = Cast<AlocomotionsystemCharacter>(detect.GetActor());
		if (detect.GetActor())
		{

			if (locoactor)
			{
				canattack = true;
				
			}
			
			
		
		}
		else
		{
			canattack = false;
		}
	}
	
	
}

void Aenemy_cpp::ragdoll()
{
	if (health <= 0)
	{
		GetMesh()->SetSimulatePhysics(true);
	}
}

void Aenemy_cpp::target()
{

	
	FVector start = GetActorLocation();
	FVector end=GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	float yaw=UKismetMathLibrary::FindLookAtRotation(start, end).Yaw;
	
	
	//canattack == false
	if (alarm==true)
	{
		SetActorRotation(FRotator(0, yaw, 0));
	}

}





// Called when the game starts or when spawned
void Aenemy_cpp::BeginPlay()
{
	Super::BeginPlay();

	

	
	
	
	
	 
	
	
	
}

// Called every frame
void Aenemy_cpp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//linetrace
	trace();
	//ragdoll
	ragdoll();
	//targetlock
	target();
	
}

// Called to bind functionality to input
void Aenemy_cpp::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

