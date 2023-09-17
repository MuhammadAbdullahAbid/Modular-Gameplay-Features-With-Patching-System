// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MGFImplementation : ModuleRules
{
	public MGFImplementation(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
		PrivateDependencyModuleNames.AddRange(new string[] { "ChunkDownloader", "Http","ModularGameplay", "GameFeatures", "GameplayAbilities", "GameplayTags", "GameplayTasks" });
	}
}
