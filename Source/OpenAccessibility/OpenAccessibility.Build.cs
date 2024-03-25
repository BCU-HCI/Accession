// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class OpenAccessibility : ModuleRules
{
	public OpenAccessibility(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);

        // Add Private KismetNodes and MaterialNodes to the project.
        /*
		string PrivateGraphEditorPath = Path.Combine(Path.GetFullPath(Target.RelativeEnginePath), "Source/Editor/GraphEditor/Private");
		*/

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
				// Internal Plugin Modules
				"OpenAccessibilityCommunication",

				// Core Modules
				"CoreUObject",
				"Engine",

				// Editor Modules
				"UnrealEd",
                "GraphEditor",
                "Kismet",

				// Slate UI
                "Slate",
				"SlateCore",
				"EditorStyle",
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
