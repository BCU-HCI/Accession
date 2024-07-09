// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class OpenAccessibilityCommunication : ModuleRules
{
	public OpenAccessibilityCommunication(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);

        PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// Internal Plugin Dependencies
				"OpenAccessibilityAnalytics",

				// Internal ThirdParty Dependencies
				"ZeroMQ",

				// Core Modules
				"CoreUObject",
				"Engine",
				"Json",

				// Editor Modules
				"UnrealEd",
                "Projects",

				// Slate UI Modules
                "Slate",
				"SlateCore",

				// Audio Modules
				"AudioMixer",
				"AudioCaptureCore",
				"AudioCapture",
				"InputCore",
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		CircularlyReferencedDependentModules.AddRange(
			new string[]
			{

            }
		);
	}
}
