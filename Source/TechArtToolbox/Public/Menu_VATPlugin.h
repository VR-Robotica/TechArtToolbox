// Copyright (c) 2026 VR Robotica. Licensed under the MIT License (see LICENSE).

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Widgets/SCompoundWidget.h"

/**
 * Toolbox "VAT" tab. The VAT page needs Unreal's AnimToTexture plugin, so it lives in the TechArtToolboxVAT module
 * and is only loaded while that plugin is enabled. Otherwise this shows a card that enables AnimToTexture in the
 * project file and restarts the editor.
 */
class SMenu_VATPlugin : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMenu_VATPlugin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	enum class EState : uint8
	{
		Disabled,		// installed with the engine, not enabled for this project
		RestartNeeded,	// enabled in the .uproject, takes effect after a restart
		Missing,		// not found in this engine
		LoadFailed		// enabled, but the VAT module didn't load
	};
	EState State = EState::Disabled;

	FText StatusText;
	bool bStatusIsError = false;

	FText GetMessage() const;
	FReply OnEnablePlugin();
	FReply OnRestartEditor();
};
