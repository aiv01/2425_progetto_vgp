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
				"Projects"
            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...	
            }
			);

        if (Target.bBuildEditor) {
            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "EditorFramework",
                "UnrealEd",
                "ToolMenus",
                "EditorScriptingUtilities"
            });

            PublicDependencyModuleNames.Add("EditorScriptingUtilities"); // Needed in Public section
        }


        DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
