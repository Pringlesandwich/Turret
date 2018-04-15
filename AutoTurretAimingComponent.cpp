// Copyright Jason Wood - JasonWoodProgrammer@gmail.com

#include "AutoTurretAimingComponent.h"
#include "AutoTurretRotator.h"
#include "AutoTurretBarrel.h"
#include "AutoTurretProjectile.h"



// Sets default values for this component's properties
UAutoTurretAimingComponent::UAutoTurretAimingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


//Getters
bool UAutoTurretAimingComponent::GetUsingClassList() { return bUsingClassList; }
bool UAutoTurretAimingComponent::GetUsingTagList() { return bUsingTagList; }
bool UAutoTurretAimingComponent::GetIsInstantHit() { return bIsInstantHit; }
FName UAutoTurretAimingComponent::GetTargetTagName() { return TargetTagName; }
float UAutoTurretAimingComponent::GetShotsPerSecond() {	return ShotsPerSecond; }
float UAutoTurretAimingComponent::GetLaserLenght(){ return LaserLenght; }


//Setters
void UAutoTurretAimingComponent::SetUsingClassList(bool Delta) { bUsingClassList = Delta; }
void UAutoTurretAimingComponent::SetUsingTagList(bool Delta) { bUsingTagList = Delta; }
void UAutoTurretAimingComponent::SetProjectileSpeed(float NewSpeed) { ProjectileSpeed = NewSpeed; }
void UAutoTurretAimingComponent::SetProjectileCount(int NewCount) {  ProjectileCount = NewCount; }
void UAutoTurretAimingComponent::SetIsInstantHit(bool Delta) { bIsInstantHit = Delta; }
void UAutoTurretAimingComponent::SetTargetTagName(FName NewName) { TargetTagName = NewName;  }
void UAutoTurretAimingComponent::SetPitchInaccuracy(float NewAccuracy) { TurretPitchAccuracy = NewAccuracy; }
void UAutoTurretAimingComponent::SetYawInaccuracy(float NewAccuracy) { TurretYawAccuracy = NewAccuracy; }
void UAutoTurretAimingComponent::SetGravityOverride(float GravityAmount) { GravityOverride = GravityAmount;  }
void UAutoTurretAimingComponent::SetLaserLenght(float NewLenght) { LaserLenght = NewLenght; }
void UAutoTurretAimingComponent::SetUsingHighArch(bool Delta) { bUsingHighArch = Delta; }


void UAutoTurretAimingComponent::setGunProperties(bool UsingPhysicalProjectiles, bool UsingHighArch, float SetGravityOverride, float SetYawInaccuracy, float SetPitchInaccuracy)
{
	bIsInstantHit = !UsingPhysicalProjectiles;
	bUsingHighArch = UsingHighArch;
	GravityOverride = SetGravityOverride;
	TurretYawAccuracy = SetYawInaccuracy;
	TurretPitchAccuracy = SetPitchInaccuracy;
}

void UAutoTurretAimingComponent::setProjectileProperties(float NewProjectileSpeed, float AmountOfProjectiles)
{
	ProjectileSpeed = NewProjectileSpeed;
	ProjectileCount = AmountOfProjectiles;
}

void UAutoTurretAimingComponent::Initialise(UAutoTurretRotator * TurretRotatorToSet, UAutoTurretBarrel * TurretBarrelToSet)
{
	ATRotator = TurretRotatorToSet;
	ATBarrel = TurretBarrelToSet;
	DefaultScanDirection = ATRotator->GetForwardVector();
}

void UAutoTurretAimingComponent::LaserSight()
{
	FVector StartTrace = ATBarrel->GetSocketLocation(FName("Projectile"));
	FVector EndTrace = StartTrace + (ATBarrel->GetForwardVector() * LaserLenght);
	DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, true, 0.01f);
}

bool UAutoTurretAimingComponent::IsTurretTracking()
{
	if (TurretState != ETurretState::Idle)
	{
		auto BarrelFwd = ATBarrel->GetForwardVector();
		return !BarrelFwd.Equals(AimDirection, 0.05);
	}
	return true;
}

void UAutoTurretAimingComponent::Fire()
{
	for (int i = 0; i < ProjectileCount; i++)
	{
		if (GetIsInstantHit())
		{
			InstantFire();
		}
		else
		{
			ProjectileFire();
		}
	}
}

void UAutoTurretAimingComponent::InstantFire()
{
	FVector StartTrace = ATBarrel->GetSocketLocation(FName("Projectile"));
	FVector EndTrace = StartTrace + (ATBarrel->GetForwardVector() * 1000);
	FVector AimOffset = EndTrace - StartTrace;

	FRotator AimRotator = AimOffset.Rotation();

	AimRotator.Pitch += FMath::RandRange(-TurretPitchAccuracy, TurretPitchAccuracy);
	AimRotator.Yaw += FMath::RandRange(-TurretYawAccuracy, TurretYawAccuracy);

	AimOffset = AimRotator.Vector();

	EndTrace = StartTrace + AimOffset * 10000.0f;
	
	//Debug only - delete to add own bullet trace
	FHitResult* HitResult = new FHitResult();
	DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, true, 0.2f);

}

void UAutoTurretAimingComponent::ProjectileFire()
{
	FRotator AimRotator = ATBarrel->GetSocketRotation(FName("Projectile"));
	AimRotator.Pitch += FMath::RandRange(-TurretPitchAccuracy, TurretPitchAccuracy);
	AimRotator.Yaw += FMath::RandRange(-TurretYawAccuracy, TurretYawAccuracy);

	if (ProjectileToSpawn)
	{
		auto Projectile = GetWorld()->SpawnActor<AAutoTurretProjectile>(ProjectileToSpawn, ATBarrel->GetSocketLocation(FName("Projectile")), AimRotator );
		Projectile->LaunchProjectile(ProjectileSpeed);
	}
}

void UAutoTurretAimingComponent::SetTargets()
{
	if ( GetUsingClassList() && !GetUsingTagList() )
	{
		Targets.Empty();
		for (int i = 0; i < TargetClasses.Num(); i++)
		{
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), TargetClasses[i], TargetsBuffer);
			for (int j = 0; j < TargetsBuffer.Num(); j++)
			{
				Targets.AddUnique(TargetsBuffer[j]);
			}
		}
	}

	if ( !GetUsingClassList() && GetUsingTagList() )
	{
		Targets.Empty();
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), GetTargetTagName(), Targets); 
	}

	if (!GetUsingClassList() && !GetUsingTagList()) UE_LOG(LogTemp, Warning, TEXT("You haven't selected a 'SetTarget' method"));
	if (GetUsingClassList() && GetUsingTagList()) UE_LOG(LogTemp, Warning, TEXT("You have selected both 'SetTarget' methods. Please select only one"));

	if (TargetExceptions.Num() > 0)
	{
		for (int k = 0; k < TargetExceptions.Num(); k++)
		{
			Targets.Remove(TargetExceptions[k]);
		}
	}

	if (Targets.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Your Target list is empty! Make sure you add classes to the turret or add tags to Actors if using tags"));
	}
}

FVector UAutoTurretAimingComponent::GetClosestTarget()
{
	if (!bUsingConeOfSight)
	{
		ValidTargets = Targets;
	}

	if (TargetExceptions.Num() > 0)
	{
		for (int k = 0; k < TargetExceptions.Num(); k++)
		{
			ValidTargets.Remove(TargetExceptions[k]);
		}
	}

	ClosestTargetDistance = MaxViewDistance;

	bool TargetInRange = false;

	for (int i = 0; i < ValidTargets.Num(); i++)
	{
		FHitResult HitResult;
		FVector StartTrace = ATBarrel->GetSocketLocation(FName("Projectile"));
		FVector EndTrace = ValidTargets[i]->GetActorLocation();
		FCollisionQueryParams* TraceParams = new FCollisionQueryParams(); 

		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility, *TraceParams))
		{
			FVector CompareVector = StartTrace - EndTrace;
			float DistanceBetweenLocations = CompareVector.Size(); 

			if (DistanceBetweenLocations < ClosestTargetDistance)
			{
				TargetInRange = true;
				ClosestTargetDistance = DistanceBetweenLocations;
				AimDirection = HitResult.Location;

				CurrentTarget = ValidTargets[i]; //this is only used in BLueprints by user

			}
		}
	}

	if (TargetInRange)
	{
		TurretState = ETurretState::Tracking;
		ATRotator->bTurretScanning = false;
	}
	else
	{
		TurretState = ETurretState::Idle;
		ATRotator->bTurretScanning = true;
	}

	return AimDirection;
}

void UAutoTurretAimingComponent::TurnToTarget(FVector AimDirection)
{
	FVector AimDelta;

	if (TurretState == ETurretState::Tracking || TurretState == ETurretState::Locked)
	{

		FirstScan = true;

		AimDelta = TurnToFiringArch(AimDirection);

		if (TurretState != ETurretState::Idle)
		{
			auto BarrelRotator = ATBarrel->GetForwardVector().Rotation();
			auto AimAsRotator = AimDelta.Rotation();
			auto DeltaRotator = AimAsRotator - BarrelRotator;

			TurnTurret(DeltaRotator);
		}
		else
		{
			TurnToScan();
		}
	}
	else
	{
		if (FirstScan)
		{
			MoveToDefaults();
		}
		else
		{
			TurnToScan();
		}
	}
}

FVector UAutoTurretAimingComponent::TurnToFiringArch(FVector DeltaAim)
{
	FVector HitLocation;
	FVector OutLaunchVelocity;
	FVector StartLocation = ATBarrel->GetSocketLocation(FName("Projectile"));

	bool bHaveAimSolution = UGameplayStatics::SuggestProjectileVelocity
	(
		this,				// World Context
		OutLaunchVelocity,  // Toss Velocity (which is returned)
		StartLocation,		// Projectile Socket
		DeltaAim,			// End Location
		ProjectileSpeed,	// Speed of projectile
		bUsingHighArch,		// using high arch
		0,					// collision radius
		GravityOverride,	// how much gravity to account for? 0-1
		ESuggestProjVelocityTraceOption::DoNotTrace // Paramater must be present to prevent bug
	);

	if (bHaveAimSolution)
	{
		AimDirection = OutLaunchVelocity.GetSafeNormal();
		return AimDirection;
	}

	TurretState = ETurretState::Idle;
	return FVector();
}

void UAutoTurretAimingComponent::TurnToScan()
{
	FVector DeltaEuler = DefaultScanDirection.Rotation().Euler();
	FRotator AimAsRotator = DeltaEuler.Rotation();

	if (bRotatePositive)
	{
		FVector DeltaEulerPlus = FVector(DeltaEuler.X, DeltaEuler.Y, DeltaEuler.Z + ScanMax);
		DeltaEulerPlus = FQuat().MakeFromEuler(DeltaEulerPlus).Vector();
		AimAsRotator = DeltaEulerPlus.Rotation();
	}
	else
	{
		FVector DeltaEulerMinus = FVector(DeltaEuler.X, DeltaEuler.Y, DeltaEuler.Z + ScanMin);
		DeltaEulerMinus = FQuat().MakeFromEuler(DeltaEulerMinus).Vector();
		AimAsRotator = DeltaEulerMinus.Rotation();
	}

	FRotator RotatorRot = ATRotator->GetSocketRotation(FName("BarrelSocket"));
	bool RotPlus = RotatorRot.Equals(AimAsRotator, 0.05);

	if (RotPlus && bRotatePositive)
	{
		bRotatePositive = false;
	}
	else if (RotPlus && !bRotatePositive)
	{
		bRotatePositive = true;
	}

	auto DeltaRotation = AimAsRotator - RotatorRot;
	TurnTurret(DeltaRotation);
}

void UAutoTurretAimingComponent::MoveToDefaults()
{
	FRotator DefaultRot = DefaultScanDirection.Rotation();
	DefaultRot.Pitch = DefaultRot.Pitch + ScanHeight;
	auto BarrelRotator = ATBarrel->GetForwardVector().Rotation();
	auto DeltaRotator = DefaultRot - BarrelRotator;

	if (BarrelRotator.Equals(DefaultRot, 0.1))
	{
		FirstScan = false;
		return;
	}

	TurnTurret(DeltaRotator);
}

void UAutoTurretAimingComponent::TurnTurret(FRotator DeltaRotator)
{

	if (ATBarrel != nullptr)
	{
		ATBarrel->Elevate(DeltaRotator.Pitch);
	}

	//rotate dependent on fastest route
	if (ATRotator != nullptr)
	{
		if (FMath::Abs(DeltaRotator.Yaw) < 180)
		{
			ATRotator->Rotate(DeltaRotator.Yaw);
		}
		else
		{
			ATRotator->Rotate(-DeltaRotator.Yaw);
		}
	}
}

void UAutoTurretAimingComponent::AddToValidTargets(AActor* ActorToAdd)
{
	ValidTargets.AddUnique(ActorToAdd);
}

void UAutoTurretAimingComponent::ClearValidTargets()
{
	ValidTargets.Empty();
}
