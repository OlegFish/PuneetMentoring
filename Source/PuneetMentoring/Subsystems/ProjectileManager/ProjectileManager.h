#pragma once

#include "CoreMinimal.h"
#include "ProjectileRuntimeData.h"
#include "Subsystems/WorldSubsystem.h"
#include "ProjectileManager.generated.h"

UCLASS()
class PUNEETMENTORING_API UProjectileManager : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UProjectileManager();
	void Add(const FVector& Location, const FVector& Velocity, float LifeTimeLeft, float MaxSpeed, float HomingAccelerationMagnitude, bool
		IsHoming, TWeakObjectPtr<AActor> Owner, TWeakObjectPtr<AActor> ProjectileActor, const TWeakObjectPtr<AActor> Target =
			nullptr);

	UFUNCTION(BlueprintCallable, Category = "Projectile manager settings")
	void DebugPrintStatus() const;

	UFUNCTION(BlueprintCallable, Category = "Projectile manager settings")
	TArray<FVector> GetProjectilesLocations() const;

protected:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	void RemoveAt(const int32 RemovedProjectileIndex);
	void Update(const float DeltaTime);
	bool HitTest(FVector PreviousLocation, FVector CurrentLocation) const;
	static void ApplyComplexHomingToProjectileVelocity(float DeltaTime, FProjectileRuntimeData& UpdatedProjectile);
	static void ApplySimpleHomingToProjectileVelocity(float DeltaTime, FProjectileRuntimeData& UpdatedProjectile);

	TArray<FProjectileRuntimeData> ProjectilePool{};
	int32 ProjectilesInUse = 0;
};
