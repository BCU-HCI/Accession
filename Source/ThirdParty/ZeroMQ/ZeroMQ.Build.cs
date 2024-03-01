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
        string ConfigurationLibDir = "";
        string LibName = "";

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            ConfigurationLibDir = Path.Combine(ZeroMQLibDir, "Windows", "x64");

            LibName = (Target.Configuration == UnrealTargetConfiguration.Debug || Target.Configuration == UnrealTargetConfiguration.Development)
                ? "libzmq-mt-sgd-4_3_5.lib" // Debug or Development Configuration
                : "libzmq-mt-s-4_3_5.lib";  // Shipping Configuration
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            ConfigurationLibDir = Path.Combine(ZeroMQLibDir, "Linux");
            LibName = "libzmq.so";
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            ConfigurationLibDir = Path.Combine(ZeroMQLibDir, "MacOS");
            LibName = "libzmq.a";
        }
        
        if (!string.IsNullOrEmpty(LibName))
        {
            PublicAdditionalLibraries.Add(Path.Combine(ConfigurationLibDir, LibName));

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