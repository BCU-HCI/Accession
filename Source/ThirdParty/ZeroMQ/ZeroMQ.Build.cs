// Copyright F-Dudley. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class ZeroMQ : ModuleRules
{
    protected virtual string ZeroMQThidPartyDir { get { return ModuleDirectory; } }
    protected virtual string ZeroMQIncludeDir { get { return Path.Combine(ZeroMQThidPartyDir, "include"); } }
    protected virtual string ZeroMQLibDir { get { return Path.Combine(ZeroMQThidPartyDir, "lib"); } }


    public ZeroMQ(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        if (Target.Platform == UnrealTargetPlatform.Win64 || 
            Target.Platform == UnrealTargetPlatform.Linux || 
            Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicIncludePaths.Add(ZeroMQIncludeDir);
            AddZeroMQLib(Target);
        }
        else
        {
            PublicDefinitions.Add("WITH_ZEROMQ=0");
        }
    }

    private void AddZeroMQLib(ReadOnlyTargetRules Target)
    {
        string StaticLib = "";

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            StaticLib = (Target.Configuration == UnrealTargetConfiguration.Debug || Target.Configuration == UnrealTargetConfiguration.Development)
                ? "libzmq-mt-sgd-4_3_5.lib" // Debug or Development Configuration Lib
                : "libzmq-mt-s-4_3_5.lib";  // Shipping Configuration Lib

            string WindowsLibDir = Path.Combine(ZeroMQLibDir, "Windows", "x64");

            Console.WriteLine("|| ZeroMQ: For Windows x64, Using Path: " + Path.Combine(WindowsLibDir, StaticLib) + " ||");
            PublicAdditionalLibraries.Add(Path.Combine(WindowsLibDir, StaticLib));
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            StaticLib = "libzmq.so";

            PublicAdditionalLibraries.Add(Path.Combine(ZeroMQLibDir, "Linux", StaticLib));
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            StaticLib = "libzmq.a";

            PublicAdditionalLibraries.Add(Path.Combine(ZeroMQLibDir, "MacOS", StaticLib));
        }
        
        if (!string.IsNullOrEmpty(StaticLib))
        {
            PublicAdditionalLibraries.Add(StaticLib);

            // Internal ZeroMQ Definitions,
            // required for ZeroMQ to work.
            PublicDefinitions.Add("ZMQ_BUILD_DRAFT_API=1");
            PublicDefinitions.Add("ZMQ_STATIC");

            // External ZeroMQ Definitions
            PublicDefinitions.Add("WITH_ZEROMQ=1"); // Can be used to check if ZeroMQ is enabled,
                                                    // in unreal scripts.
        }
        else
        {
            PublicDefinitions.Add("WITH_ZEROMQ=0");
        }
    }
}