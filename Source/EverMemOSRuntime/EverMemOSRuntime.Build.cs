// Copyright TonyL. All Rights Reserved.

using UnrealBuildTool;

public class EverMemOSRuntime : ModuleRules
{
	public EverMemOSRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine"
		});

			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"HTTP",
				"Json",
				"JsonUtilities",
				"DeveloperSettings"
			});
		}
	}
