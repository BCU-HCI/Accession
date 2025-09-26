// Copyright (C) HCI-BCU. All rights reserved.
// Published under GPLv3 License, 2025. See LICENSE in the Plugin Root for more information.

using System.IO;
using UnrealBuildTool;

public class Accession : ModuleRules
{
	public Accession(ReadOnlyTargetRules Target) : base(Target)
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
				// Internal Plugin Modules
				"AccessionAnalytics",
				"AccessionCommunication",

				// Core Modules
				"CoreUObject",
				"Engine",
				"Json",

				// Editor Modules
				"UnrealEd",
				"GraphEditor",
				"Kismet",
				"AIModule",

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
