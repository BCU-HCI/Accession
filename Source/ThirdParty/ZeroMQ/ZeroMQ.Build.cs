// Copyright F-Dudley. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class ZeroMQ : ModuleRules
{
    protected virtual string ZeroMQThidPartyDir { get {  return "$(ModuleDir)"; } }
    protected virtual string ZeroMQIncludeDir { get { return Path.Combine(ZeroMQThidPartyDir, "include"); } }
    protected virtual string ZeroMQLibDir { get { return Path.Combine(ZeroMQThidPartyDir, "lib"); } }

    public ZeroMQ(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        string LibraryDir = "";

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string LibraryName = (Target.Configuration == UnrealTargetConfiguration.Debug && Target.bDebugBuildsActuallyUseDebugCRT)
                ? "libzmq-mt-sgd-4_3_5.lib"
                : "libzmq-mt-s-4_3_5.lib";

            LibraryDir = Path.Combine("Windows", "x64", LibraryName);

            PublicDefinitions.Add("WITH_ZEROMQ=1");
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            string LibraryName = "libzmq.so";
            LibraryDir = Path.Combine("Linux", LibraryName);

            PublicDefinitions.Add("WITH_ZEROMQ=1");
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            string LibraryName = "libzmq.a";
            LibraryDir = Path.Combine("Mac", LibraryName);

            PublicDefinitions.Add("WITH_ZEROMQ=1");
        }
        else
        {
            PublicDefinitions.Add("WITH_ZEROMQ=0");
        }

        PublicIncludePaths.Add(ZeroMQIncludeDir);
        PublicAdditionalLibraries.Add(Path.Combine(ZeroMQLibDir, LibraryDir));
    }
}