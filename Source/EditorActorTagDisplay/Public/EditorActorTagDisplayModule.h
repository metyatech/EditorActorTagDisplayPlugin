// Copyright 2026 Udon-Tobira. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FEditorActorTagDisplayController;

class FEditorActorTagDisplayModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    TUniquePtr<FEditorActorTagDisplayController> Controller;
};
