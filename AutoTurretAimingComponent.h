// Copyright Jason Wood - JasonWoodProgrammer@gmail.com

#pragma once

#include "Engine.h"
#include "Components/ActorComponent.h"
#include "AutoTurretAimingComponent.generated.h"


// Turret States
UENUM(BlueprintType)
enum class ETurretState : uint8
{
	Idle,
	Tracking,
	Locked
};

//Forward declaration
class UAutoTurretRotator;
class UAutoTurretBarrel;
class AAutoTurretProjectile; // note - others are U because they are component - this is A because it's an Actor

/**
* Class for calculating aiming and firing functionality
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TURRET_API UAutoTurretAimingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UAutoTurretAimingComponent();

	// An array of targets set from SetTargets()
	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> Targets; 

	// Used as a buffer to send list of avaliable targets dependent on bUsingConeOfSight
	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> ValidTargets;
	
	// Used as a buffer to send targets dependent on bUsingClassList or bUsingTagList
	UPROPERTY() 
	TArray<AActor*> TargetsBuffer; 

	// List of Classes set by user
	UPROPERTY(EditAnywhere, Category = "Setup")
	TArray<UClass*> TargetClasses;

	//Add specific actors here that you do not want the turret to target
	UPROPERTY(EditAnywhere, Category = "Setup")
	TArray<AActor*> TargetExceptions;

	// The current targeted actor, not used in C++ but can be used in Blueprints
	UPROPERTY(BlueprintReadWrite)
	AActor* CurrentTarget;

	// Loops for the amount of projectiles and calls either InstantFire or ProjectileFire
	UFUNCTION(BlueprintCallable, Category = "AI")
	void Fire();

	// Fires a RayTrace with accuracy
	UFUNCTION(BlueprintCallable, Category = "AI")
	void InstantFire();

	// Spawns a class set by user
	UFUNCTION(BlueprintCallable, Category = "AI")
	void ProjectileFire();

	// Collects all targets in world based on Class list or Tag list
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetTargets();

	/**
	* Creates a raytrace to each valid target and returns a world location
	* @return the location of the valid target closest to the turret
	*/
	UFUNCTION(BlueprintCallable, Category = "AI")
	FVector GetClosestTarget();

	/**
	* Turns to either an enemy target or set scan postion
	* @param AimDirection - Location from GetClosetTarget(), used to potentially pass to TurnToFiringArch()
	*/
	UFUNCTION(BlueprintCallable, Category = "AI")
	void TurnToTarget(FVector AimDirection);

	/**
	* Calculates and returns a FVector dependent on firing arch settings
	* @param DeltaAim - Location in which the firing arch will use to calculate
	* @return an FVector in which the turret will need to face to hit target
	*/
	UFUNCTION(BlueprintCallable, Category = "AI")
	FVector TurnToFiringArch(FVector DeltaAim);

	// Governs the scan angle and switches between scan side
	UFUNCTION(BlueprintCallable, Category = "AI")
	void TurnToScan();

	/**
	* Takes in a FRotator and calls function on AutoTurretRotator and AutoTurretBarrel to move
	* @param DeltaRotator - FRotator used as target to communicate with AutoTurretRotator and AutoTurretBarrel 
	*/
	UFUNCTION(BlueprintCallable, Category = "AI")
	void TurnTurret(FRotator DeltaRotator);

	/**
	* Adds a single actor to the valid target list, called in Blueprints
	* @param ActorToAdd - Adds a single actor to the list, is deleted after each tick
	*/
	UFUNCTION(BlueprintCallable)
	void AddToValidTargets(AActor* ActorToAdd);

	// Clears all targets from ValidTarget list, called in Blueprints
	UFUNCTION(BlueprintCallable)
	void ClearValidTargets();

	// Checks to see if the barrel vector is pointing at the target 
	UFUNCTION(BlueprintCallable, Category = "State")
	bool IsTurretTracking();

	/**
	* Assigns Objects from the Blueprint to the forward declerated classes, called in Blueprints
	* @param TurretRotatorToSet - Takes in a component from Blueprint class to establish connection 
	* @param TurretBarrelToSet - Takes in a component from Blueprint class to establish connection 
	*/
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void Initialise(UAutoTurretRotator * TurretRotatorToSet, UAutoTurretBarrel * TurretBarrelToSet);

	// Enables the C++ class to spawn a Blueprint of the class AutoTurretProjectile
	UPROPERTY(EditAnywhere)
	TSubclassOf<AAutoTurretProjectile> ProjectileToSpawn;

	// Creates a debug line that is used as a lasersight
	UFUNCTION(BlueprintCallable, Category = "ToChange")
	void LaserSight();

	// On initial change to state "idle" the turret will first move to its default position, adds a ddefault scan height
	UFUNCTION(BlueprintCallable)
	void MoveToDefaults();

	// Of UEnem class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	ETurretState TurretState = ETurretState::Idle;


private:


	//GETTERS

	UFUNCTION(BlueprintCallable, Category = "Get/Set")
	bool GetUsingClassList();

	UFUNCTION(BlueprintCallable, Category = "Get/Set")
	bool GetUsingTagList();

	UFUNCTION(BlueprintCallable, Category = "Get/Set")
	bool GetIsInstantHit();

	UFUNCTION(BlueprintCallable, Category = "Get/Set")
	FName GetTargetTagName();

	UFUNCTION(BlueprintCallable, Category = "Get/Set")
	float GetShotsPerSecond();

	UFUNCTION(BlueprintCallable, Category = "Get/Set")
	float GetLaserLenght();


	//SETTERS

	UFUNCTION(BlueprintCallable, Category = "Get/Set")
	void SetUsingClassList(bool Delta);

	UFUNCTION(BlueprintCallable, Category = "Get/Set")
	void SetUsingTagList(bool Delta);

	UFUNCTION(BlueprintCallable, Category = "Get/Set")
	void SetProjectileSpeed(float NewSpeed);

	UFUNCTION(BlueprintCallable, Category = "Get/Set")
	void SetProjectileCount(int NewCount);

	UFUNCTION(BlueprintCallable, Category = "Get/Set")
	void SetIsInstantHit(bool Delta);

	UFUNCTION(BlueprintCallable, Category = "Get/Set")
	void SetTargetTagName(FName NewName);

	UFUNCTION(BlueprintCallable, Category = "Get/Set")
	void SetPitchInaccuracy(float NewAccuracy);

	UFUNCTION(BlueprintCallable, Category = "Get/Set")
	void SetYawInaccuracy(float NewAccuracy);

	UFUNCTION(BlueprintCallable, Category = "Get/Set")
	void SetGravityOverride(float GravityAmount);

	UFUNCTION(BlueprintCallable, Category = "Get/Set")
	void SetUsingHighArch(bool Delta);

	UFUNCTION(BlueprintCallable, Category = "Get/Set")
	void SetLaserLenght(float NewLenght);

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void setGunProperties(bool UsingPhysicalProjectiles, bool UsingHighArch, float SetGravityOverride, float SetYawInaccuracy, float SetPitchInaccuracy);

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void setProjectileProperties(float NewProjectileSpeed, float AmountOfProjectiles);


	//Properties

	// True if you want to use the cone as a sight for the turret
	UPROPERTY(EditAnywhere, Category = "Setup") 
	bool bUsingConeOfSight;

	// Turret will search world for classes with this tag
	UPROPERTY(EditAnywhere, Category = "Setup")
	FName TargetTagName = "TurretTarget";

	// For forward decleration of AutoTurretRotator
	UPROPERTY()
	UAutoTurretRotator* ATRotator = nullptr;

	// For forward decleration of AutoTurretBarrel
	UPROPERTY()
	UAutoTurretBarrel* ATBarrel = nullptr;
	
	// User set class that the turret will spawn - must be child of AutoTurretProjectile C++ class
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<AAutoTurretProjectile> AutoTurretProjectileBlueprint;

	// If true then the Turret will search game for TargetClasses[], pick either this or Using Tag List
	UPROPERTY(EditAnywhere, Category = "Setup") 
	bool bUsingClassList;

	// If true then the Turret will search game for Actors with the "TargetTagName" set by you, pick either this or Using Class List
	UPROPERTY(EditAnywhere, Category = "Setup") 
	bool bUsingTagList;

	// If true then the turret will not spawn a projectile but trace to the actor, false it will spawn set projectile
	UPROPERTY(EditAnywhere, Category = "Setup") 
	bool bIsInstantHit;

	// How many times a second the turret will fire
	UPROPERTY(EditAnywhere, Category = "Setup") 
	float ShotsPerSecond;

	// How far the turret can check for targets (MUST BE BIGGER THAN SIZE OF CONE IF USING CONE)
	UPROPERTY(EditAnywhere, Category = "Setup") 
	float MaxViewDistance;

	// used as a temporary buffer to find the closest target
	UPROPERTY()
	float ClosestTargetDistance;

	// FVector used by all the aiming functions to calculate where the turret needs to move
	UPROPERTY()
	FVector AimDirection;

	// 0 is fully accurate, number of degrees the turret may fire from target
	UPROPERTY(EditAnywhere, Category = "Setup") 
	float TurretPitchAccuracy;

	// 0 is fully accurate, number of degrees the turret may fire from target
	UPROPERTY(EditAnywhere, Category = "Setup") 
	float TurretYawAccuracy;

	// Speed at which the projectile fires 
	UPROPERTY(EditAnywhere, Category = "Setup|Projectile") 
	float ProjectileSpeed;

	// Number of projectiles the turret spawns on firing
	UPROPERTY(EditAnywhere, Category = "Setup|Projectile") 
	int ProjectileCount = 1;

	// True and the turret will fire at the slower higher arch for projectile trajectory
	UPROPERTY(EditAnywhere, Category = "Setup") 
	bool bUsingHighArch;

	// 0 - Account for gravity in aim, 1 - ignore gravity
	UPROPERTY(meta = (ClampMin = 0), meta = (ClampMax = 1), EditAnywhere, Category = "Setup") 
	float GravityOverride;

	// Lenght of Laser Sight Beam
	UPROPERTY(EditAnywhere, Category = "Setup") 
	float LaserLenght;

	// Logic used by ScanToTarget to switch between sides
	UPROPERTY() 
	bool bRotatePositive = true; 

	// The Direction in which the turret will scan
	UPROPERTY(EditAnywhere, Category = "Setup|Scanning") 
	FVector DefaultScanDirection;

	// Hold initial scan direction on begin play
	UPROPERTY() 
	FRotator ScanRotation;

	// the maximum positive scan distance (cant be greater than Rotator Max Degrees)
	UPROPERTY(EditAnywhere, Category = "Setup|Scanning") 
	float ScanMax;

	// the minimum positive scan distance (cant be less than Rotator Min Degrees) 
	UPROPERTY(EditAnywhere, Category = "Setup|Scanning") 
	float ScanMin;

	// The angle at which the turret is when scanning. + is up, - is down.
	UPROPERTY(EditAnywhere, Category = "Setup|Scanning") 
	float ScanHeight;

	// The speed at which the turret moves degres per second when scanning
	UPROPERTY(EditAnywhere, Category = "Setup|Scanning") 
	float ScanSpeed;

	// Used as logic by TurnToScan in order to establish initial action to take
	UPROPERTY()
	bool FirstScan = true;
	
};
