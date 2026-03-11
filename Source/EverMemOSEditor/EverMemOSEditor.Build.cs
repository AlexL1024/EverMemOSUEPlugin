// Copyright TonyL. All Rights Reserved.

using UnrealBuildTool;

public class EverMemOSEditor : ModuleRules
{
	public EverMemOSEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"EverMemOSRuntime",
			"UnrealEd",
			"Slate",
			"SlateCore"
		});
	}
}
