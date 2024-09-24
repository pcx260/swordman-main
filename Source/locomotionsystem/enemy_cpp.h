// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "enemy_cpp.generated.h"

UCLASS()
class LOCOMOTIONSYSTEM_API Aenemy_cpp : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	Aenemy_cpp();

	UPROPERTY(BlueprintReadWrite)
	float health;

	UPROPERTY(BlueprintReadWrite)
	bool alarm;

	UPROPERTY(BlueprintReadWrite)
	bool isattack;

	UPROPERTY(BlueprintReadWrite)
	bool takedamage;

	UPROPERTY(BlueprintReadWrite)
	bool candamage;

	UPROPERTY(BlueprintReadWrite)
	bool canattack;
	void trace();
	void ragdoll();
	void target();

	
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
