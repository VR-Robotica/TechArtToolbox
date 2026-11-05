// Copyright (c) 2026 VR Robotica. Licensed under the MIT License (see LICENSE).

/*
 * TechArtToolboxVAT: the Toolbox's VAT page (Menu_VATBake) and its baking (Core_VATBake).
 *
 * Kept apart from the TechArtToolbox module because it links Unreal's AnimToTexture plugin. It has no loading
 * phase: the Toolbox's VAT tab (SMenu_VATPlugin) loads it only while AnimToTexture is enabled.
 */

#include "TechArtToolboxVATModule.h"
#include "Menu_VATBake.h"
#include "Modules/ModuleManager.h"

class FTechArtToolboxVATModule : public ITechArtToolboxVATModule
{
public:
	virtual TSharedRef<SWidget> CreatePage() override
	{
		return SNew(SMenu_VATBake);
	}
};

IMPLEMENT_MODULE(FTechArtToolboxVATModule, TechArtToolboxVAT)
