// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WaveTool : ModuleRules
{
	public WaveTool(ReadOnlyTargetRules Target) : base(Target)
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
				// ... add other public dependencies that you statically link with here ...
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"InputCore",
				"Projects",
				"BlueprintGraph"	
            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...	
            }
			);


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Slate",
				"SlateCore",
				"PropertyEditor",
				"EditorFramework",
				"UnrealEd",
				"ToolMenus",
				"EditorScriptingUtilities"
            });

        PrivateIncludePathModuleNames.AddRange(new string[] {
            "PropertyEditor"
        });

        if (Target.bBuildEditor)
        {
            // Still keep UnrealEd or other Editor modules here if needed
            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "UnrealEd",
                "EditorScriptingUtilities"
            });
        }

        DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
