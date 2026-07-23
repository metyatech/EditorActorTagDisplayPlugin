# Settings Reference

## Project Settings

Open **Project Settings > Plugins > Actor Metadata Overlay**.

| Setting | Type | Default | Description |
| --- | --- | --- | --- |
| Rules | Array of rules | One default Actor rule | Rules are evaluated from top to bottom; only the first match is used. |
| Default Display Template | String | `{ActorLabel}` / `Class` / `Tags` on three lines | Used when a matching rule has an empty `DisplayTemplate`. |
| Outline Color | Linear color | Black | Outline color used when user outlines are enabled. |
| Max Property Value Length | Integer | 120 | Clamped to 16–1024 characters for each property value. |

## Editor Preferences

Open **Editor Preferences > Plugins > Actor Metadata Overlay**.

| Setting | Type | Default | Description |
| --- | --- | --- | --- |
| Display Mode | Off / Selected Actors / All Matching Actors | Selected Actors | Controls which cached actors are drawn. |
| Global Max Draw Distance | Float | 10000 | A value of 0 disables the global distance limit. A rule value greater than 0 overrides it. |
| Text Scale | Float | 1.0 | Clamped to 0.25–4.0. |
| Outlined | Boolean | true | Draws text outlines using the project outline color. |
| Draw Bounding Boxes | Boolean | true | Enables boxes for rules that also enable their box option. |

## Rule Fields

| Field | Type | Default | Description |
| --- | --- | --- | --- |
| Rule Name | Name | `New Rule` | Label used to identify the rule and unresolved-class warnings. |
| Enabled | Boolean | true | Disabled rules never match. |
| Actor Class | Soft class | Empty | Required class filter. An empty or unresolved class never matches. |
| Include Derived Classes | Boolean | true | Uses `IsA` when true; uses exact class equality when false. |
| Required Actor Tags | Name array | Empty | Every tag must be present. |
| Excluded Actor Tags | Name array | Empty | Any present tag rejects the rule. |
| Display Template | String | Empty | Overrides the project template when non-empty. |
| Display Color | Linear color | White | Text and optional bounding-box color. |
| World Offset | Vector | `(0, 0, 20)` | Added to the calculated actor anchor. |
| Max Draw Distance | Float | 0 | A value of 0 uses the global preference. |
| Selected Only | Boolean | false | Requires the actor to be selected in every display mode. |
| Draw Bounding Box | Boolean | false | Draws the actor AABB when the user preference also allows boxes. |

## Template Tokens

| Token | Value |
| --- | --- |
| `{ActorLabel}` | `AActor::GetActorLabel()` |
| `{ActorName}` | `AActor::GetName()` |
| `{ActorClass}` | Actor class name, with a display-only `_C` suffix removed. |
| `{ActorTags}` | Actor Tags sorted case-insensitively and joined by `, `. |
| `{GameplayTags}` | Owned Gameplay Tags from `IGameplayTagAssetInterface`, sorted and joined by `, `. |
| `{Folder}` | Actor folder path. |
| `{DataLayers}` | Actor data layer names, sorted and joined by `, `. |
| `{Property:PropertyName}` | A supported direct public property value. |

Unknown tokens become `<unknown:TokenName>`. An unclosed `{` remains literal text.

## Property Restrictions

Property tokens accept only direct top-level property names. Dotted paths, array indexes, and function-like expressions are rejected. A property must have `CPF_Edit` or `CPF_BlueprintVisible`, and must not be transient or deprecated. No getter, function, or Blueprint VM is executed.

Supported types are booleans, signed and unsigned integers, floats, doubles, enums, enum bytes, `FName`, `FString`, `FText`, UObject references, soft object references, and structs. Values are trimmed, line breaks and tabs become spaces, and values longer than the configured maximum end in `...`.

## Rule Priority

Rules are evaluated in array order. The first enabled rule satisfying class, inheritance, required-tag, and excluded-tag conditions owns the actor. Later matching rules are ignored.

## Viewport Behavior

Overlays appear in normal Level Editor Viewports, including Perspective, Top, Front, Side, and split Level Editor layouts. They remain visible with **Game View** off and are hidden with **Game View** on, in PIE or SIE game screens, and in Static Mesh Editor or other Asset Preview Viewports. The **Data Layers** token is non-empty only when the actor belongs to data layers in a World Partition map.
