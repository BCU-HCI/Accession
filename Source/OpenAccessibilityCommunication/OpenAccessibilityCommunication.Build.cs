// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

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
				//"OpenAccessibility",

				// Internal ThirdParty Dependencies
				"ZeroMQ",

				// Unreal Dependencies
				"CoreUObject",
				"Engine",
				"UnrealEd",

				"Slate",
				"SlateCore",

				"AudioMixer",
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
