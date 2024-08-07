// Copyright F-Dudley. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class ZeroMQ : ModuleRules
{
	bool IsDebug
	{
		get
		{
			return Target.Configuration == UnrealTargetConfiguration.Debug ||
			       Target.Configuration == UnrealTargetConfiguration.DebugGame;
		}
	}

	protected virtual string PlatformLibDir
	{
		get { return Path.Combine(ModuleDirectory, "lib", Target.Platform.ToString()); }
	}

	protected virtual string PlatformBinariesDir
	{
		get { return Path.Combine(PluginDirectory, "Binaries", "ThirdParty", "ZeroMQ", Target.Platform.ToString());  }
	}

    public ZeroMQ(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicSystemIncludePaths.Add(
            Path.Combine(ModuleDirectory, "include")
        );

        if (AddPlatformLib())
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

    private bool AddPlatformLib()
    {
        if (Target.Platform == UnrealTargetPlatform.Win64)
	    {
            AddWindowsLib();
		    return true;
	    }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            AddLinuxLib();
	        return true;
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            AddMacLib();
	        return true;
        }

	    return false;
    }

    private void AddWindowsLib()
    {
        string fileName = IsDebug
	        ? "libzmq-mt-gd-4_3_5"
	        : "libzmq-mt-4_3_5";

        string WindowsLibDir = IsDebug
	        ? Path.Combine(PlatformLibDir, "debug")
	        : PlatformLibDir;

        PublicAdditionalLibraries.Add(
	        Path.Combine(WindowsLibDir, fileName + ".lib")
		);

        PublicDelayLoadDLLs.Add(fileName + ".dll");

        RuntimeDependencies.Add(
            Path.Combine(PlatformBinariesDir, fileName + ".dll"),
            Path.Combine(WindowsLibDir, fileName + ".dll")
        );
    }

    private void AddLinuxLib()
    {
	    string soFile = "libzmq-mt-4_3_5.so";

        PublicAdditionalLibraries.Add(
	        Path.Combine(PlatformLibDir, soFile)
		);

        PublicDelayLoadDLLs.Add(soFile);

        RuntimeDependencies.Add(
	        Path.Combine(PlatformBinariesDir, soFile), 
	        Path.Combine(PlatformLibDir, soFile)
		);
    }

    private void AddMacLib()
    {
	    string dylibName = "libzmq-mt-4_3_5.dylib";

        PublicDelayLoadDLLs.Add(
			Path.Combine(PlatformLibDir, dylibName)    
		);

        RuntimeDependencies.Add(
	        Path.Combine(PlatformBinariesDir, dylibName),
            Path.Combine(PlatformLibDir, dylibName)
		);
    }
}