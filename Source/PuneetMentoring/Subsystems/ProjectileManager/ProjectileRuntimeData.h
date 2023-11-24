#pragma once

class AActor;

struct PUNEETMENTORING_API FProjectileRuntimeData
{
	FVector Location;
	FVector Velocity;
	float LifeTimeLeft;
	float MaxSpeed;
	float HomingAccelerationMagnitude;
	bool IsHoming;
	TWeakObjectPtr<AActor> Owner;
	TWeakObjectPtr<AActor> ProjectileActor;
	TWeakObjectPtr<AActor> Target;

	explicit FProjectileRuntimeData(
		const FVector& Location = FVector::ZeroVector,
		const FVector& Velocity = FVector::ZeroVector,
		const float LifeTimeLeft = 0.f,
		const float MaxSpeed = 0.f,
		const float HomingAccelerationMagnitude = 0.f,
		const bool IsHoming = false,
		const TWeakObjectPtr<AActor>& Owner = nullptr,
		const TWeakObjectPtr<AActor>& ProjectileActor = nullptr,
		const TWeakObjectPtr<AActor>& Target = nullptr)
		: Location(Location)
		, Velocity(Velocity)
		, LifeTimeLeft(LifeTimeLeft)
		, MaxSpeed(MaxSpeed)
		, HomingAccelerationMagnitude(HomingAccelerationMagnitude)
		, IsHoming(IsHoming)
		, Owner(Owner)
		, ProjectileActor(ProjectileActor)
		, Target(Target) { }
};
