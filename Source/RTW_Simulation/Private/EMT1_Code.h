// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Animation/BlendSpace1D.h"
#include "Runtime/Engine/Classes/Engine/TargetPoint.h"
#include "EMT1_Code.generated.h"

UCLASS()
class AEMT1_Code : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEMT1_Code();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float timeAccumulated;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* EMT1_Mesh;
	
	// EMT1 has a rather simple interaction: stand at patients feet, do some idle stuff, walk to the head, do another idle stuff
	// First code to test if behaviour is as intended
	
	// Treating patient
	UPROPERTY(EditAnywhere, Category = "Animation")
		UAnimSequence* EMT1_TreatPatient;

	UPROPERTY(EditAnywhere, Category = "Animation")
		UAnimSequence* EMT1_IdleStand;

	UPROPERTY(BluePrintReadOnly, EditAnywhere, Category = "Animation")
		UBlendSpace1D* BlendSpaceLocomotion;

	UPROPERTY(EditAnywhere, Category = "Target Points")
		ATargetPoint* TargetPoint_XRay;

	UPROPERTY(EditAnywhere, Category = "Target Points")
		ATargetPoint* TargetPoint_PatientHead;

	UPROPERTY(EditAnywhere, Category = "Target Points")
		ATargetPoint* TargetPoint_PatientUpperBody;

	UPROPERTY(EditAnywhere, Category = "Target Points")
		ATargetPoint* TargetPoint_PatientLowerBody;

	UPROPERTY(EditAnywhere, Category = "Target Points")
		ATargetPoint* TargetPoint_IVStart;

	UPROPERTY(EditAnywhere, Category = "Target Points")
		ATargetPoint* TargetPoint_LobbyCorner;

	UPROPERTY(EditAnywhere, Category = "Target Points")
		ATargetPoint* TargetPoint_LookTarget;
};
