// Copyright 2026 Udon-Tobira. All Rights Reserved.

#include "EditorActorTagDisplayTestActor.h"

AEditorActorTagDisplayTestActor::AEditorActorTagDisplayTestActor()
{
    SetFlags(RF_Transient);
    PrimaryActorTick.bCanEverTick = false;
}

void AEditorActorTagDisplayTestActor::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
    TagContainer = GameplayTags;
}
