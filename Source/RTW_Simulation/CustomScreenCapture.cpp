// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomScreenCapture.h"
#include "RTW_WorldSettings.h"

#include <fstream>

#if __cplusplus < 201703L // If the version of C++ is less than 17
	// It was still in the experimental:: namespace
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

// Sets default values
ACustomScreenCapture::ACustomScreenCapture()
	: resolutionX(1024)
	, resolutionY(1024)
	, field_of_view(90.0f)
	, outputFolderPath(TEXT("."))
	, colorCameraOffset(0.0f, 0.0f, 0.0f)
	, counterImage(0)
	, baseFilenameDepth("")
	, baseFilenameColor("")
{
	PrimaryActorTick.bCanEverTick = true;

	// Only tick once all updates regarding movement and physics have happened
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	OurCameraDepth = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewportCameraDepth"));
	OurCameraDepth->SetupAttachment(RootComponent);

	OurCameraColor = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewportCameraColor"));
	OurCameraColor->SetupAttachment(RootComponent);

	// Resolution has to be a power of 2. This code finds the lowest RxR resolution which has more pixel than set
	uint32_t higher = resolutionX > resolutionY ? resolutionX : resolutionY;

	higher--;
	higher |= higher >> 1;
	higher |= higher >> 2;
	higher |= higher >> 4;
	higher |= higher >> 8;
	higher |= higher >> 16;
	higher++;

	internResolution = higher;

	sceneCaptureDepth = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureDepth"));
	sceneCaptureDepth->SetupAttachment(OurCameraDepth);

	sceneCaptureColor = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureColor"));
	sceneCaptureColor->SetupAttachment(OurCameraColor);
}

// Called when the game starts or when spawned
void ACustomScreenCapture::BeginPlay()
{
	Super::BeginPlay();

#pragma region Get_Resolution_Power_of_2
	// Resolution has to be a power of 2. This code finds the lowest RxR resolution which has equal or more pixel than set
	uint32_t higher = std::max(resolutionX, resolutionY);

	higher--;
	higher |= higher >> 1;
	higher |= higher >> 2;
	higher |= higher >> 4;
	higher |= higher >> 8;
	higher |= higher >> 16;
	higher++;

	internResolution = higher;
#pragma endregion

	OurCameraDepth->FieldOfView = field_of_view;
	OurCameraColor->SetRelativeLocation(colorCameraOffset);
	
	renderTargetDepth = NewObject<UTextureRenderTarget2D>();
	renderTargetDepth->InitCustomFormat(internResolution, internResolution, EPixelFormat::PF_FloatRGBA, true);

	renderTargetDepth->UpdateResourceImmediate();

	renderTargetColor = NewObject<UTextureRenderTarget2D>();
	renderTargetColor->InitCustomFormat(internResolution, internResolution, EPixelFormat::PF_B8G8R8A8, true);

	renderTargetColor->UpdateResourceImmediate();

	sceneCaptureDepth->CaptureSource = SCS_SceneDepth;
	sceneCaptureDepth->TextureTarget = renderTargetDepth;
	sceneCaptureDepth->bCaptureEveryFrame = true;
	
	sceneCaptureColor->CaptureSource = SCS_FinalColorLDR;
	sceneCaptureColor->TextureTarget = renderTargetColor;
	sceneCaptureColor->bCaptureEveryFrame = true;

	//imageRendered = sceneCaptureDepth->TextureTarget->ConstructTexture2D(this, "CameraImage", EObjectFlags::RF_NoFlags, CTF_DeferCompression);

#pragma region Get_File_Name
	// Temporary buffer
	char targetBuffer[10];

	std::ofstream metaData;
	
	// World location, as string
	std::string strPosX;
	std::string strPosY;
	std::string strPosZ;

	// World rotation, as string
	std::string strRotPitch;
	std::string strRotRoll;
	std::string strRotYaw;

	// Get world location of Actor
	FVector location = OurCameraDepth->GetComponentLocation();

	sprintf(targetBuffer, "%.3f", location.X);
	strPosX = std::string(targetBuffer);

	sprintf(targetBuffer, "%.3f", location.Y);
	strPosY = std::string(targetBuffer);

	sprintf(targetBuffer, "%.3f", location.Z);
	strPosZ = std::string(targetBuffer);

	// Get yaw pitch roll of actor
	FRotator rotation = OurCameraDepth->GetComponentRotation();

	sprintf(targetBuffer, "%.3f", rotation.Pitch);
	strRotPitch = std::string(targetBuffer);

	sprintf(targetBuffer, "%.3f", rotation.Roll);
	strRotRoll = std::string(targetBuffer);

	sprintf(targetBuffer, "%.3f", rotation.Yaw);
	strRotYaw = std::string(targetBuffer);

	baseFilenameDepth = std::string(TCHAR_TO_UTF8(*outputFolderPath));

	// Create necessary folders
	fs::create_directories(baseFilenameDepth);

	// Go on with the file name
	if (baseFilenameDepth.back() != '/')
	{
		baseFilenameDepth.append("/");
	}

	ARTW_WorldSettings* tempPtr = reinterpret_cast<ARTW_WorldSettings*>(GetWorldSettings());
	sprintf(targetBuffer, "%.3f", tempPtr->frames_per_second);
	std::string strFPS = std::string(targetBuffer);

	metaData.open(baseFilenameDepth + "Metadata.txt");

	metaData << "Frames per second: " << strFPS << "\n";

	metaData << "Depth Position X: " << strPosX << "\n";
	metaData << "Depth Position Y: " << strPosY << "\n";
	metaData << "Depth Position Z: " << strPosZ << "\n";
	
	metaData << "Depth Rotation Pitch: " << strRotPitch << "\n";
	metaData << "Depth Rotation Roll: " << strRotRoll << "\n";
	metaData << "Depth Rotation Yaw: " << strRotYaw << "\n";

	baseFilenameDepth += std::string("image");
	baseFilenameDepth += std::string("_number_");

	location = OurCameraColor->GetComponentLocation();

	sprintf(targetBuffer, "%.3f", location.X);
	strPosX += std::string(targetBuffer);

	sprintf(targetBuffer, "%.3f", location.Y);
	strPosY = std::string(targetBuffer);

	sprintf(targetBuffer, "%.3f", location.Z);
	strPosZ = std::string(targetBuffer);

	// Get yaw pitch roll of actor
	rotation = OurCameraColor->GetComponentRotation();

	sprintf(targetBuffer, "%.3f", rotation.Pitch);
	strRotPitch = std::string(targetBuffer);

	sprintf(targetBuffer, "%.3f", rotation.Roll);
	strRotRoll = std::string(targetBuffer);

	sprintf(targetBuffer, "%.3f", rotation.Yaw);
	strRotYaw = std::string(targetBuffer);

	baseFilenameColor = std::string(TCHAR_TO_UTF8(*outputFolderPath));

	// Create necessary folders
	fs::create_directories(baseFilenameColor);

	// Go on with the file name
	if (baseFilenameColor.back() != '/')
	{
		baseFilenameColor.append("/");
	}

	baseFilenameColor += std::string("image");
	baseFilenameColor += std::string("_number_");

	metaData << "Color Position X: " << strPosX << "\n";
	metaData << "Color Position Y: " << strPosY << "\n";
	metaData << "Color Position Z: " << strPosZ << "\n";

	metaData << "Color Rotation Pitch: " << strRotPitch << "\n";
	metaData << "Color Rotation Roll: " << strRotRoll << "\n";
	metaData << "Color Rotation Yaw: " << strRotYaw;

	metaData.close();

#pragma endregion
}

// Called every frame
void ACustomScreenCapture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	sCounter = std::to_string(counterImage);
	sCounter = std::string(6 - sCounter.length(), '0') + sCounter;
	
	if (counterImage > 0)
	{
		SaveTextureDepthmap();
		SaveTextureColor();
	}

	counterImage++;
}

void ACustomScreenCapture::SaveTextureDepthmap()
{
	auto RenderTargetResource = renderTargetDepth->GameThread_GetRenderTargetResource();

	if (RenderTargetResource)
	{
		TArray<FFloat16Color> buffer16;
		RenderTargetResource->ReadFloat16Pixels(buffer16);

		std::string fileName = baseFilenameDepth;
		fileName += sCounter + std::string(".raw32f");
		std::ofstream targetFileDepth(fileName, std::ofstream::binary);
		
		depthVector.resize(buffer16.Num());

		for (int32_t index = 0; index < buffer16.Num(); index++)
		{
			depthVector[index] = buffer16[index].R.GetFloat();
		}

		targetFileDepth.write(reinterpret_cast<char*>(&depthVector[0]), depthVector.size() * sizeof(depthVector[0]));
		targetFileDepth.close();
	}
}

void ACustomScreenCapture::SaveTextureColor()
{
	auto RenderTargetResource = renderTargetColor->GameThread_GetRenderTargetResource();

	if (RenderTargetResource)
	{
		TArray<FColor> buffer8;
		RenderTargetResource->ReadPixels(buffer8);

		std::string fileName = baseFilenameColor;
		fileName += sCounter + std::string(".raw8");
		std::ofstream targetFileColor(fileName, std::ofstream::binary);
		
		targetFileColor.write(reinterpret_cast<char*>(buffer8.GetData()), buffer8.Num() * sizeof(FColor));
		targetFileColor.close();
	}
}