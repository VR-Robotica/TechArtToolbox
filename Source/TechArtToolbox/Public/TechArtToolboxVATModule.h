// Copyright (c) 2026 VR Robotica. Licensed under the MIT License (see LICENSE).

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class SWidget;

/**
 * The VAT page lives in its own module (TechArtToolboxVAT) because it links Unreal's AnimToTexture plugin.
 * The Toolbox loads that module only while AnimToTexture is enabled, so the rest of the Toolbox works without it
 * (see SMenu_VATPlugin).
 */
class ITechArtToolboxVATModule : public IModuleInterface
{
public:
	static inline const FName ModuleName = TEXT("TechArtToolboxVAT");

	/** The plugin the VAT page needs. */
	static inline const FString RequiredPlugin = TEXT("AnimToTexture");

	/** A new VAT page widget. */
	virtual TSharedRef<SWidget> CreatePage() = 0;
};
