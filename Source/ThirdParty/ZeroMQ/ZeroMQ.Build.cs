// Copyright F-Dudley. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class ZeroMQ : ModuleRules
{
    public ZeroMQ(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicSystemIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));

        if (AddZeroMQLib())
        {
            // Internal ZeroMQ Definitions,
            // required for ZeroMQ to work.
            PublicDefinitions.Add("ZMQ_BUILD_DRAFT_API=1");

            // External ZeroMQ Definitions
            PublicDefinitions.Add("WITH_ZEROMQ=1"); // Can be used to check if ZeroMQ is enabled,
													// in unreal scripts.
        }
        else
        {
            PublicDefinitions.Add("WITH_ZEROMQ=0");
        }
    }

    private bool AddZeroMQLib()
    {
        if (Target.Platform == UnrealTargetPlatform.Win64)
	    {
		    string FileName = (Target.Configuration == UnrealTargetConfiguration.Debug)
			    ? "libzmq-mt-gd-4_3_5"
			    : "libzmq-mt-4_3_5";

		    string PlatformLibDir = Path.Combine(ModuleDirectory, "lib", Target.Platform.ToString());

            // Add Import Library
            {
			    string LibPath = Path.Combine(PlatformLibDir, FileName + ".lib");
			    Console.WriteLine("|| ZEROMQ BUILD || Lib-Path: " + LibPath);

			    PublicAdditionalLibraries.Add(LibPath);
            }

            {
	            string DllName = FileName + ".dll";

                // Delay-Load the DLL
	            Console.WriteLine("|| ZEROMQ BUILD || Delay-DLL-Name: " + DllName);

                PublicDelayLoadDLLs.Add(DllName);

                // Ensure that the DLL is staged with the executable
                RuntimeDependencies.Add(
	                Path.Combine(ModuleDirectory, 
								 "Binaries\\ThirdParty\\ZeroMQ", 
								 Target.Platform.ToString(),
								 DllName
                    )
				);
            }

		    return true;
	    }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {


	        return true;
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {


	        return true;
        }

	    return false;
    }
}