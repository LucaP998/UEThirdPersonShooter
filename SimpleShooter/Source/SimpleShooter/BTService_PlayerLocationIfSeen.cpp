// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_PlayerLocationIfSeen.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_PlayerLocationIfSeen::UBTService_PlayerLocationIfSeen()
{
    NodeName=TEXT("Player Location If Seen");
}



void UBTService_PlayerLocationIfSeen::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds){
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if(Player!=nullptr && AIController->LineOfSightTo(Player)){
        OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), Player);
    }
    else{
        OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
    }
    
}