// Copyright 2026 Udon-Tobira. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "EditorActorTagDisplayTestActor.h"

#include "EditorActorTagDisplayRuleMatcher.h"
#include "EditorActorTagDisplaySettings.h"
#include "EditorActorTagDisplayTemplateFormatter.h"
#include "GameplayTagsManager.h"
#include "Misc/AutomationTest.h"

namespace
{
    AEditorActorTagDisplayTestActor* MakeTestActor()
    {
        return NewObject<AEditorActorTagDisplayTestActor>(GetTransientPackage(), NAME_None, RF_Transient);
    }

    FResolvedActorMetadataOverlayRule MakeRule(UClass* ActorClass)
    {
        FResolvedActorMetadataOverlayRule Rule;
        Rule.Rule.bEnabled = true;
        Rule.Rule.ActorClass = ActorClass;
        Rule.ResolvedActorClass = ActorClass;
        return Rule;
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEditorActorTagDisplayRuleMatchingTest,
                                 "EditorActorTagDisplay.RuleMatching",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEditorActorTagDisplayRuleMatchingTest::RunTest(const FString& Parameters)
{
    AEditorActorTagDisplayTestActor* Actor = MakeTestActor();
    Actor->Tags = {FName(TEXT("Required")), FName(TEXT("Other"))};

    TArray<FResolvedActorMetadataOverlayRule> Rules;
    FResolvedActorMetadataOverlayRule DisabledRule = MakeRule(AActor::StaticClass());
    DisabledRule.Rule.bEnabled = false;
    Rules.Add(DisabledRule);

    FResolvedActorMetadataOverlayRule FirstRule = MakeRule(AEditorActorTagDisplayTestActor::StaticClass());
    FirstRule.Rule.RequiredActorTags = {FName(TEXT("Required"))};
    Rules.Add(FirstRule);

    FResolvedActorMetadataOverlayRule LaterRule = MakeRule(AActor::StaticClass());
    Rules.Add(LaterRule);

    TestEqual(TEXT("The first enabled matching rule wins"), EditorActorTagDisplayRuleMatcher::FindMatchingRule(*Actor, Rules), 1);

    FirstRule.Rule.bIncludeDerivedClasses = false;
    Rules[1] = FirstRule;
    TestEqual(TEXT("Exact class matching accepts the exact class"), EditorActorTagDisplayRuleMatcher::FindMatchingRule(*Actor, Rules), 1);

    FirstRule.ResolvedActorClass = AActor::StaticClass();
    FirstRule.Rule.ActorClass = AActor::StaticClass();
    FirstRule.Rule.bIncludeDerivedClasses = false;
    Rules[1] = FirstRule;
    TestEqual(TEXT("Exact class matching rejects a derived class"), EditorActorTagDisplayRuleMatcher::FindMatchingRule(*Actor, Rules), 2);

    FirstRule.Rule.bIncludeDerivedClasses = true;
    Rules[1] = FirstRule;
    TestEqual(TEXT("Derived class matching accepts a derived class"), EditorActorTagDisplayRuleMatcher::FindMatchingRule(*Actor, Rules), 1);

    FirstRule.Rule.RequiredActorTags = {FName(TEXT("Required")), FName(TEXT("Missing"))};
    Rules[1] = FirstRule;
    TestEqual(TEXT("All required tags are required"), EditorActorTagDisplayRuleMatcher::FindMatchingRule(*Actor, Rules), 2);

    FirstRule.Rule.RequiredActorTags = {FName(TEXT("Required"))};
    FirstRule.Rule.ExcludedActorTags = {FName(TEXT("Other"))};
    Rules[1] = FirstRule;
    TestEqual(TEXT("Any excluded tag rejects a rule"), EditorActorTagDisplayRuleMatcher::FindMatchingRule(*Actor, Rules), 2);

    FResolvedActorMetadataOverlayRule EmptyClassRule = MakeRule(nullptr);
    EmptyClassRule.ResolvedActorClass = nullptr;
    EmptyClassRule.Rule.ActorClass.Reset();
    Rules.Insert(EmptyClassRule, 0);
    TestEqual(TEXT("A null class rule never matches"), EditorActorTagDisplayRuleMatcher::FindMatchingRule(*Actor, Rules), 3);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEditorActorTagDisplayFixedTemplateTokensTest,
                                 "EditorActorTagDisplay.FixedTemplateTokens",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEditorActorTagDisplayFixedTemplateTokensTest::RunTest(const FString& Parameters)
{
    AEditorActorTagDisplayTestActor* Actor = MakeTestActor();
    Actor->SetActorLabel(TEXT("Display Label"));
    Actor->Tags = {FName(TEXT("zeta")), FName(TEXT("Alpha"))};

    UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
    FGameplayTagContainer AvailableGameplayTags;
    GameplayTagsManager.RequestAllGameplayTags(AvailableGameplayTags, true);
    TArray<FGameplayTag> SortedGameplayTags;
    AvailableGameplayTags.GetGameplayTagArray(SortedGameplayTags);
    SortedGameplayTags.Sort([](const FGameplayTag& Left, const FGameplayTag& Right)
    {
        return Left.ToString().Compare(Right.ToString(), ESearchCase::IgnoreCase) < 0;
    });

    if (SortedGameplayTags.Num() >= 2)
    {
        Actor->GameplayTags.AddTag(SortedGameplayTags[1]);
        Actor->GameplayTags.AddTag(SortedGameplayTags[0]);
    }

    TSet<FString> Warnings;
    const FString Template = TEXT("{ActorLabel}\n{ActorName}\n{ActorClass}\n{ActorTags}\n{GameplayTags}\n{Folder}");
    const FString Output = FEditorActorTagDisplayTemplateFormatter::Format(*Actor, Template, 120, Warnings);
    TestTrue(TEXT("The label is present"), Output.Contains(TEXT("Display Label")));
    TestTrue(TEXT("Actor tags use case-insensitive ordering"), Output.Contains(TEXT("Alpha, zeta")));
    if (SortedGameplayTags.Num() >= 2)
    {
        const FString ExpectedGameplayTags = SortedGameplayTags[0].ToString() + TEXT(", ") + SortedGameplayTags[1].ToString();
        TestTrue(TEXT("Gameplay tags use case-insensitive ordering"), Output.Contains(ExpectedGameplayTags));
    }
    else
    {
        TestTrue(TEXT("The gameplay tag token is resolved when the engine tag dictionary is empty"), !Output.Contains(TEXT("{GameplayTags}")));
    }
    TestTrue(TEXT("The folder token is resolved for a world-less test actor"), !Output.Contains(TEXT("{Folder}")));
    TestTrue(TEXT("Template newlines are preserved"), Output.Contains(TEXT("Display Label\n")));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEditorActorTagDisplayTemplateValidationTest,
                                 "EditorActorTagDisplay.TemplateValidation",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEditorActorTagDisplayTemplateValidationTest::RunTest(const FString& Parameters)
{
    AEditorActorTagDisplayTestActor* Actor = MakeTestActor();
    TSet<FString> Warnings;
    const FString Output = FEditorActorTagDisplayTemplateFormatter::Format(
        *Actor, TEXT("{UnknownToken}|{Property:Missing}|{Property:TestString.Value}|literal {"), 120, Warnings);
    TestTrue(TEXT("Unknown tokens are explicit"), Output.Contains(TEXT("<unknown:UnknownToken>")));
    TestTrue(TEXT("Missing properties are explicit"), Output.Contains(TEXT("<missing:Missing>")));
    TestTrue(TEXT("Nested properties are rejected"), Output.Contains(TEXT("<unsupported:TestString.Value>")));
    TestTrue(TEXT("Unclosed braces remain literal"), Output.EndsWith(TEXT("literal {")));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEditorActorTagDisplayPropertyFormattingTest,
                                 "EditorActorTagDisplay.PropertyFormatting",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEditorActorTagDisplayPropertyFormattingTest::RunTest(const FString& Parameters)
{
    AEditorActorTagDisplayTestActor* Actor = MakeTestActor();
    Actor->ObjectReference = nullptr;
    Actor->TestString = TEXT("Line one\nLine two\twith tab");
    TSet<FString> Warnings;

    const FString Output = FEditorActorTagDisplayTemplateFormatter::Format(
        *Actor,
        TEXT("{Property:bTestBool}|{Property:TestInteger}|{Property:TestFloat}|{Property:TestDouble}|{Property:TestName}|{Property:TestString}|{Property:TestText}|{Property:TestEnum}|{Property:ObjectReference}|{Property:TestStruct}|{Property:PrivateProperty}|{Property:TransientString}"),
        120,
        Warnings);
    TestTrue(TEXT("Boolean formatting is stable"), Output.Contains(TEXT("true")));
    TestTrue(TEXT("Integer formatting is stable"), Output.Contains(TEXT("42")));
    TestTrue(TEXT("Floating point formatting is stable"), Output.Contains(TEXT("1.5")));
    TestTrue(TEXT("Null object references are explicit"), Output.Contains(TEXT("None")));
    TestTrue(TEXT("Struct formatting is present"), Output.Contains(TEXT("X=1")));
    TestTrue(TEXT("Non-public properties are rejected"), Output.Contains(TEXT("<unsupported:PrivateProperty>")));
    TestTrue(TEXT("Transient properties are rejected"), Output.Contains(TEXT("<unsupported:TransientString>")));
    TestFalse(TEXT("Newlines and tabs are normalized"), Output.Contains(TEXT("\n")));

    const FString ShortOutput = FEditorActorTagDisplayTemplateFormatter::Format(*Actor, TEXT("{Property:TestString}"), 16, Warnings);
    TestTrue(TEXT("Long properties are truncated with an ellipsis"), ShortOutput.Len() <= 16 && ShortOutput.EndsWith(TEXT("...")));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEditorActorTagDisplayDefaultConfigurationTest,
                                 "EditorActorTagDisplay.DefaultConfiguration",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEditorActorTagDisplayDefaultConfigurationTest::RunTest(const FString& Parameters)
{
    const UEditorActorTagDisplayUserSettings* UserSettings = GetDefault<UEditorActorTagDisplayUserSettings>();
    const UEditorActorTagDisplayProjectSettings* ProjectSettings = GetDefault<UEditorActorTagDisplayProjectSettings>();
    TestEqual(TEXT("Default display mode is Selected"), UserSettings->DisplayMode, EActorMetadataOverlayMode::Selected);
    TestEqual(TEXT("Default text scale is one"), UserSettings->TextScale, 1.0f);
    TestEqual(TEXT("Default distance is 10000"), UserSettings->GlobalMaxDrawDistance, 10000.0f);
    TestEqual(TEXT("Default project template is stable"), ProjectSettings->DefaultDisplayTemplate, FString(TEXT("{ActorLabel}\nClass: {ActorClass}\nTags: {ActorTags}")));
    TestEqual(TEXT("One default rule is present"), ProjectSettings->Rules.Num(), 1);
    if (ProjectSettings->Rules.Num() == 1)
    {
        TestEqual(TEXT("Default rule targets Actor"), ProjectSettings->Rules[0].ActorClass.Get(), AActor::StaticClass());
    }
    return true;
}

#endif
