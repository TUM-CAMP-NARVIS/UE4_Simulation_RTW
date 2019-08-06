// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "RTW_WorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class ARTW_WorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	ARTW_WorldSettings();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Simulation Settings")
		float frames_per_second;
};
