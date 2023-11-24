#include "ProjectileLauncher.h"
#include "ProjectileManager.h"
#include "ProjectileData.h"

UProjectileLauncher::UProjectileLauncher() {}

void UProjectileLauncher::FireProjectile()
{
	ensureMsgf(ProjectileData, TEXT("ProjectileData missing"));

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid World in UProjectileLauncher::Fire"));
		return;
	}

	// Create projectile actor and init it with mesh
	const FTransform SpawnTransform = FTransform(GetComponentRotation(), GetComponentLocation());

	// Register created projectile actor with manager
	UProjectileManager* ProjectileManager = World->GetSubsystem<UProjectileManager>();
	if (!IsValid(ProjectileManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid ProjectileManager in UProjectileLauncher::Fire"));
		return;
	}

	AActor* ProjectileOwner = GetOwner();
	if (!IsValid(ProjectileOwner))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid ProjectileOwner in UProjectileLauncher::Fire"));
		return;
	}

	if (Target != nullptr && !IsValid(Target))
	{
		Target = nullptr;
	}

	ProjectileManager->Add(GetComponentLocation(), GetForwardVector() * ProjectileData->InitialSpeed, 20.0f, ProjectileData->MaxSpeed, ProjectileData->HomingAccelerationMagnitude, ProjectileData->IsHoming, ProjectileOwner, nullptr, Target);
}
