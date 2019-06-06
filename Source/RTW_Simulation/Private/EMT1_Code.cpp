// Fill out your copyright notice in the Description page of Project Settings.


#include "EMT1_Code.h"

// Sets default values
AEMT1_Code::AEMT1_Code() 
	: timeAccumulated(0.0f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.EndTickGroup = TG_PostPhysics;
	
}

// Called when the game starts or when spawned
void AEMT1_Code::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEMT1_Code::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEMT1_Code::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

