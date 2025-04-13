// Some copyright should be here...

// � Manuel Solano
// � Alessandro Caccamo
// � Claudio Dallai

using UnrealBuildTool;
using System.IO;

public class PNetworking : ModuleRules
{
    public PNetworking(ReadOnlyTargetRules Target) : base(Target)
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
                "OnlineSubsystem",
                "OnlineSubsystemUtils",
            }
        );
        
        // To add .lib files
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Public", "steam", "lib", "steam_api64.lib"));
        
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
            }
        );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
                "OnlineSubsystemSteam"
            }
        );
    }
}