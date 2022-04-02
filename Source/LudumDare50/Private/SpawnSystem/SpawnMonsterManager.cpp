// Copyright (c) 2022. Made by Title Goose Team during LudumDare 50. All rights reserved. 


#include "SpawnSystem/SpawnMonsterManager.h"

#include "EngineUtils.h"
#include "Characters/EnemyCharacter.h"
#include "SpawnSystem/SpawnMonsterActor.h"

DECLARE_LOG_CATEGORY_CLASS(LogSpawnMonsterManager, All, All);

ASpawnMonsterManager::ASpawnMonsterManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASpawnMonsterManager::CallSpawn()
{
	UE_LOG(LogSpawnMonsterManager, Display, TEXT("Count Spawn!"));
	for (int i = 0; i < WaveData.MaxAtTime; i++)
	{

		const int32 LastIndex = SpawnActors.Num() - 1;
		for (int32  y = 0; y <= LastIndex; ++y)
		{
			const int32 Index = FMath::RandRange(y, LastIndex);
			if (y != Index) { SpawnActors.Swap(y, Index); }
		}
		
		for (ASpawnMonsterActor* SpawnActor : SpawnActors)
		{
			if (SpawnActor->Spawn(WaveData.Enemies[WaveData.Enemies.Num() - 1]))
			{
				WaveData.Spawned++;
				WaveData.Enemies.RemoveAt(WaveData.Enemies.Num() - 1);
				break;
			}
		}
	}
	
	if (WaveData.MaxEnemies == WaveData.Spawned)
	{
		GetWorldTimerManager().ClearTimer(WaveTimerHandle);
		GetWorldTimerManager().SetTimer(RoundTimerHandle, this, &ASpawnMonsterManager::CallRound, RoundTimeDelay, false);
	}
}

void ASpawnMonsterManager::CallRound()
{
	UE_LOG(LogSpawnMonsterManager, Display, TEXT("Count Round!"));
	WaveData = GenerateEnemies();
	GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &ASpawnMonsterManager::CallSpawn, WaveDelayTime, true);
}

FWaveData ASpawnMonsterManager::GenerateEnemies()
{
	TArray<TSubclassOf<AEnemyCharacter>> Enemies;
	
	TArray<FEnemyData*> AllEnemies;
	Wave->GetAllRows<FEnemyData>("", AllEnemies);
	

	for (const FEnemyData* EnemyStat : AllEnemies)
	{
		for (int i = 0; i < EnemyStat->Count; i++ )
		{
			Enemies.Add(EnemyStat->Enemy);
		}
	}

	const int32 LastIndex = Enemies.Num() - 1;
	for (int32  i = 0; i <= LastIndex; ++i)
	{
		const int32 Index = FMath::RandRange(i, LastIndex);
		if (i != Index) { Enemies.Swap(i, Index); }
	}

	return {
		Enemies,
		Enemies.Num(),
		0,
		WaveDelayTime,
		WaveMaxAtTime
	};
}

void ASpawnMonsterManager::BeginPlay()
{
	Super::BeginPlay();

	SpawnActors.Reset();
	for (TActorIterator<ASpawnMonsterActor> It(GetWorld()); It; ++It) { SpawnActors.Add(*It); }
	UE_LOG(LogSpawnMonsterManager, Display, TEXT("Count Spawn: %i."), SpawnActors.Num());
	
	GetWorldTimerManager().SetTimer(RoundTimerHandle, this, &ASpawnMonsterManager::CallRound, WaveDelayTime, false);
}
