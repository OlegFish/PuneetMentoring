#include "ProjectileManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"

void UProjectileManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Update(DeltaTime);
}

TStatId UProjectileManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UProjectileManager, STATGROUP_Tickables)
}

UProjectileManager::UProjectileManager()
{
	ProjectilePool.Reserve(100);
}

void UProjectileManager::Add(const FVector& Location, const FVector& Velocity, float LifeTimeLeft, float MaxSpeed, float HomingAccelerationMagnitude, bool IsHoming, TWeakObjectPtr<AActor> Owner, TWeakObjectPtr<AActor> ProjectileActor, const TWeakObjectPtr<AActor> Target)
{
	if (ProjectilesInUse < ProjectilePool.Num())
	{
		ProjectilePool[ProjectilesInUse] = FProjectileRuntimeData(Location, Velocity, LifeTimeLeft, MaxSpeed, HomingAccelerationMagnitude, IsHoming, Owner, ProjectileActor, Target);
	}
	else
	{
		ProjectilePool.Emplace(Location, Velocity, LifeTimeLeft, MaxSpeed, HomingAccelerationMagnitude, IsHoming, Owner, ProjectileActor, Target);
	}
	ProjectilesInUse++;
}

void UProjectileManager::RemoveAt(const int32 RemovedProjectileIndex)
{
	if (!ProjectilePool.IsValidIndex(RemovedProjectileIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid RemovedProjectileIndex in UProjectileManager::RemoveAt"));
		return;
	}

	FProjectileRuntimeData& RemovedProjectile = ProjectilePool[RemovedProjectileIndex];
	if (RemovedProjectile.ProjectileActor.IsValid())
	{
		RemovedProjectile.ProjectileActor->Destroy();
		RemovedProjectile.ProjectileActor.Reset();
	}
	RemovedProjectile.Owner.Reset();

	if (ProjectilesInUse > 1)
	{
		ProjectilePool[RemovedProjectileIndex] = ProjectilePool[ProjectilesInUse - 1];
		ProjectilesInUse--;
	}
	else if (ProjectilesInUse == 1)
	{
		ProjectilesInUse = 0;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid ProjectilesInUse in UProjectileManager::RemoveAt"));
	}
}

void UProjectileManager::Update(const float DeltaTime)
{
	if (ProjectilesInUse == 0)
	{
		return;
	}

	ensureMsgf(ProjectilesInUse >= 0 && ProjectilesInUse <= ProjectilePool.Num(), TEXT("Invalid ProjectilesInUse in UProjectileManager::Update"));

	for (int32 UpdatedProjectileIndex = ProjectilesInUse - 1; UpdatedProjectileIndex >= 0; --UpdatedProjectileIndex)
	{
		FProjectileRuntimeData& UpdatedProjectile = ProjectilePool[UpdatedProjectileIndex];

		UpdatedProjectile.LifeTimeLeft -= DeltaTime;
		if (UpdatedProjectile.LifeTimeLeft < 0)
		{
			RemoveAt(UpdatedProjectileIndex);
			continue;
		}

		const FVector PreviousLocation = UpdatedProjectile.Location;
		if (UpdatedProjectile.IsHoming)
		{
			ApplySimpleHomingToProjectileVelocity(DeltaTime, UpdatedProjectile);
		}
		UpdatedProjectile.Location += UpdatedProjectile.Velocity * DeltaTime;

		if (HitTest(PreviousLocation, UpdatedProjectile.Location))
		{
			RemoveAt(UpdatedProjectileIndex);
		}
	}
}

void UProjectileManager::ApplySimpleHomingToProjectileVelocity(const float DeltaTime, FProjectileRuntimeData& UpdatedProjectile)
{
	if (UpdatedProjectile.Target.IsValid())
	{
		const FVector NormalizedDirectionToTarget = (UpdatedProjectile.Target->GetActorLocation() - UpdatedProjectile.Location).GetSafeNormal();
		if (FVector::DotProduct(NormalizedDirectionToTarget, UpdatedProjectile.Velocity) > 0)
		{
			const FVector Acceleration = NormalizedDirectionToTarget * UpdatedProjectile.HomingAccelerationMagnitude;
			UpdatedProjectile.Velocity += Acceleration * DeltaTime;
			UpdatedProjectile.Velocity = UpdatedProjectile.Velocity.GetSafeNormal() * UpdatedProjectile.MaxSpeed;
		}
	}
}

void UProjectileManager::ApplyComplexHomingToProjectileVelocity(const float DeltaTime, FProjectileRuntimeData& UpdatedProjectile)
{
	if (UpdatedProjectile.Target.IsValid())
	{
		const FVector NormalizedVelocity = UpdatedProjectile.Velocity.GetSafeNormal();
		const FVector NormalizedDirectionToTarget = (UpdatedProjectile.Target->GetActorLocation() - UpdatedProjectile.Location).GetSafeNormal();
		const double DotProduct = FVector::DotProduct(NormalizedDirectionToTarget, NormalizedVelocity);
		if (DotProduct > 0.90)
		{
			const FVector PerpendicularVelocityComponent = (NormalizedVelocity - NormalizedDirectionToTarget * DotProduct);
			if (PerpendicularVelocityComponent.Size() * 1000.f > 200.f * DeltaTime)
			{
				UpdatedProjectile.Velocity -= PerpendicularVelocityComponent.GetSafeNormal() * 200.f * DeltaTime;
				UpdatedProjectile.Velocity = UpdatedProjectile.Velocity.GetSafeNormal() * 1000.f;
			}
		}
	}
}

bool UProjectileManager::HitTest(FVector PreviousLocation, FVector CurrentLocation) const
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid World in UProjectileManager::HitTest"));
		return false;
	}

	FHitResult HitResult{};
	//We can add ignored actors if needed with CollisionParams.AddIgnoredActor();
	FCollisionQueryParams CollisionParams{};	

	if (World->LineTraceSingleByChannel(HitResult, PreviousLocation, CurrentLocation, ECC_Visibility, CollisionParams))
	{
		DrawDebugLine(World, PreviousLocation, CurrentLocation, FColor::Red, false, 3.f, 0, 10);
		if (AActor* HitActor = HitResult.GetActor(); IsValid(HitActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *HitActor->GetName());
		}
		return true;
	}
	return false;
}

void UProjectileManager::DebugPrintStatus() const
{
	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString::Printf(TEXT("In use / Elements / Elements + Slack: %d, %d, %d"), ProjectilesInUse, ProjectilePool.Num(), ProjectilePool.Num() + ProjectilePool.GetSlack()));
}

TArray<FVector> UProjectileManager::GetProjectilesLocations() const
{
	TArray<FVector> Output{};
	for (int32 Index = 0; Index < ProjectilesInUse; Index++)
	{
		Output.Add(ProjectilePool[Index].Location);
	}
	return Output;
}

// Vladimir recommendations: Acceleration = (Target.Location - UpdatedProjectile.CurrentLocation).GetSafeNormal()*UpdatedProjectile.HomingAccelerationMagnitude;
// Version 1:
// variant 2b:
// FVector VelocityPerpendicularComponent = (UpdatedProjectile.Velocity.GetSafeNormal() - FVector::DotProduct(UpdatedProjectile.Velocity.GetSafeNormal(), DesiredDirection) * DesiredDirection) * 1000.f;
