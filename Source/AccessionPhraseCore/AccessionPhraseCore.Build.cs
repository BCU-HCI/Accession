// Copyright (C) HCI-BCU 2026. All rights reserved.

using System.IO;
using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class AccessionPhraseCore : ModuleRules
{
	public AccessionPhraseCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
			}
			);

		PrivateIncludePaths.AddRange(
			new string[] {
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"EditorSubsystem"
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// Internal Plugin Dependencies
				"AccessionAnalytics",
				"AccessionCommunication", // THIS IS TEMPORARY, REMOVE ONCE REQUIRED FUNCTIONALITY IS MOVED TO THIS MODULE

				// Core Modules
				"CoreUObject",
				"Engine",
				"Json",

				// Editor Modules
				"UnrealEd",
				"Projects",
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);

		CircularlyReferencedDependentModules.AddRange(
			new string[]
			{

			}
		);
	}
}
