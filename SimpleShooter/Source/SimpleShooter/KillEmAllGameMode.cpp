// Fill out your copyright notice in the Description page of Project Settings.

#include "KillEmAllGameMode.h"
#include "EngineUtils.h"
#include "GameFramework/Controller.h"
#include "ShooterPlayerController.h"
#include "ShooterAIController.h"

void AKillEmAllGameMode::PawnKilled(APawn *PawnKilled)
{
    Super::PawnKilled(PawnKilled);
    APlayerController *Player = Cast<APlayerController>(PawnKilled->GetController());
    if (Player != nullptr)
    {
        EndGame(false);
    }
    for(AShooterAIController* AI: TActorRange<AShooterAIController>(GetWorld()))
    {
        if(!AI->IsDead()){
            return;
        }
    }
    EndGame(true);
}

void AKillEmAllGameMode::EndGame(bool bIsPlayerWinner)
{
    for (AController *Controller : TActorRange<AController>(GetWorld()))
    {
        bool bIsWinner = Controller->IsPlayerController() == bIsPlayerWinner;
        Controller->GameHasEnded(Controller->GetPawn(), bIsWinner);
    }
}