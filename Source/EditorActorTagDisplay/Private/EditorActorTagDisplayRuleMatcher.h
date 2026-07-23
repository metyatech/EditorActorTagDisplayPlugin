// Copyright 2026 Udon-Tobira. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorActorTagDisplayTypes.h"

class AActor;
class UClass;

struct FResolvedActorMetadataOverlayRule
{
    FActorMetadataOverlayRule Rule;
    UClass* ResolvedActorClass = nullptr;
};

namespace EditorActorTagDisplayRuleMatcher
{
    void ResolveRules(const TArray<FActorMetadataOverlayRule>& Rules,
                      TArray<FResolvedActorMetadataOverlayRule>& OutResolvedRules,
                      TSet<FName>& InOutWarnedRuleNames);

    int32 FindMatchingRule(const AActor& Actor, const TArray<FResolvedActorMetadataOverlayRule>& Rules);
}
