#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ProjectileData.generated.h"

class UStaticMesh;

UCLASS(BlueprintType, Blueprintable)
class PUNEETMENTORING_API UProjectileData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* Mesh = nullptr;

	UPROPERTY(EditDefaultsOnly)
	bool IsHoming = false;
	
	UPROPERTY(EditDefaultsOnly)
	float InitialSpeed = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxSpeed = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float HomingAccelerationMagnitude = 0.f;
};