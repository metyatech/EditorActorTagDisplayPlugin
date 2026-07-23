// Copyright 2026 Udon-Tobira. All Rights Reserved.

#include "EditorActorTagDisplayModule.h"

#include "EditorActorTagDisplayController.h"

void FEditorActorTagDisplayModule::StartupModule()
{
    Controller = MakeUnique<FEditorActorTagDisplayController>();
    Controller->Initialize();
}

void FEditorActorTagDisplayModule::ShutdownModule()
{
    if (Controller.IsValid())
    {
        Controller->Shutdown();
        Controller.Reset();
    }
}

IMPLEMENT_MODULE(FEditorActorTagDisplayModule, EditorActorTagDisplay)
