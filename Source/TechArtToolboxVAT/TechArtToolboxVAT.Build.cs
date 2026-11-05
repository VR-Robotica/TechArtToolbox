// Copyright (c) 2026 VR Robotica. Licensed under the MIT License (see LICENSE).

using UnrealBuildTool;

/**
 * TechArt Toolbox VAT page. Its own module because it links Unreal's AnimToTexture plugin: the Toolbox loads it
 * only while that plugin is enabled, and otherwise offers to enable it.
 */
public class TechArtToolboxVAT : ModuleRules
{
	public TechArtToolboxVAT(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"AnimToTexture",
			"AnimToTextureEditor",
			"AssetRegistry",
			"CoreUObject",
			"Engine",
			"InputCore",
			"MaterialEditor",
			"MeshDescription",
			"PropertyEditor",
			"RenderCore",
			"Slate",
			"SlateCore",
			"StaticMeshDescription",
			"TechArtToolbox",		// shared page widgets (MenuWidgets.h), helpers (Core_Common.h) and settings
			"UnrealEd"
		});
	}
}
