// Black Ledger - impact camera shake

#include "BLCameraShake_Impact.h"
#include "Shakes/PerlinNoiseCameraShakePattern.h"

UBLCameraShake_Impact::UBLCameraShake_Impact(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bSingleInstance = false; // overlapping hits stack naturally; scale keeps it sane

	UPerlinNoiseCameraShakePattern* Pattern =
		ObjectInitializer.CreateDefaultSubobject<UPerlinNoiseCameraShakePattern>(this, TEXT("Pattern"));
	Pattern->Duration = 0.3f;
	Pattern->BlendInTime = 0.02f;
	Pattern->BlendOutTime = 0.2f;
	Pattern->Pitch.Amplitude = 1.6f;
	Pattern->Pitch.Frequency = 22.f;
	Pattern->Yaw.Amplitude = 1.2f;
	Pattern->Yaw.Frequency = 18.f;
	Pattern->Roll.Amplitude = 0.7f;
	Pattern->Roll.Frequency = 14.f;
	Pattern->Z.Amplitude = 4.f;
	Pattern->Z.Frequency = 20.f;
	Pattern->FOV.Amplitude = 0.5f;
	Pattern->FOV.Frequency = 20.f;
	SetRootShakePattern(Pattern);
}
