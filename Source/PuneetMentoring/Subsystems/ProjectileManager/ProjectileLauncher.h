#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "ProjectileLauncher.generated.h"

class UProjectileData;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PUNEETMENTORING_API UProjectileLauncher : public UArrowComponent
{
	GENERATED_BODY()

public:
	UProjectileLauncher();

	UPROPERTY(EditDefaultsOnly)
	UProjectileData* ProjectileData{};

	UPROPERTY(EditAnywhere)
	AActor* Target{};

protected:
	UFUNCTION(BlueprintCallable, Category = "Projectile launcher")
	void FireProjectile();
};