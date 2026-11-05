// Copyright (c) 2026 VR Robotica. Licensed under the MIT License (see LICENSE).

/*
 * Toolbox > VAT tab: the AnimToTexture check.
 *
 * AnimToTexture enabled  -> load TechArtToolboxVAT and show its page.
 * Otherwise              -> one card: Enable AnimToTexture (adds it to the .uproject, like Edit > Plugins does),
 *                           then Restart Editor, since plugins only load at startup.
 */

#include "Menu_VATPlugin.h"

#include "GameProjectGenerationModule.h"
#include "Interfaces/IPluginManager.h"
#include "Interfaces/IProjectManager.h"
#include "MenuWidgets.h"
#include "Misc/Paths.h"
#include "ProjectDescriptor.h"
#include "TechArtToolboxVATModule.h"
#include "UnrealEdMisc.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"

#define LOCTEXT_NAMESPACE "SMenu_VATPlugin"

namespace
{
	/** AnimToTexture is in the project file and enabled there (it may not be loaded yet). */
	bool IsEnabledInProjectFile()
	{
		const FProjectDescriptor* Project = IProjectManager::Get().GetCurrentProject();
		if (!Project)
		{
			return false;
		}
		const FPluginReferenceDescriptor* Reference = Project->Plugins.FindByPredicate([](const FPluginReferenceDescriptor& P)
		{
			return P.Name == ITechArtToolboxVATModule::RequiredPlugin;
		});
		return Reference && Reference->bEnabled;
	}
}

void SMenu_VATPlugin::Construct(const FArguments& InArgs)
{
	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(ITechArtToolboxVATModule::RequiredPlugin);
	if (Plugin.IsValid() && Plugin->IsEnabled())
	{
		if (ITechArtToolboxVATModule* Module = FModuleManager::LoadModulePtr<ITechArtToolboxVATModule>(ITechArtToolboxVATModule::ModuleName))
		{
			ChildSlot
			[
				Module->CreatePage()
			];
			return;
		}
		State = EState::LoadFailed;
	}
	else if (!Plugin.IsValid())
	{
		State = EState::Missing;
	}
	else
	{
		State = IsEnabledInProjectFile() ? EState::RestartNeeded : EState::Disabled;
	}

	const FText PluginName = FText::FromString(ITechArtToolboxVATModule::RequiredPlugin);

	ChildSlot
	[
		VrrMenu::Stack({
			VrrMenu::HelpCard(FText::Format(LOCTEXT("PluginCard", "{0} plugin"), PluginName),
				LOCTEXT("PluginHelp", "The VAT page bakes skeletal animation into vertex animation textures with Unreal's AnimToTexture plugin. "
					"It ships with the engine but has to be enabled for the project."),
				SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					VrrMenu::Note(MakeAttributeSP(this, &SMenu_VATPlugin::GetMessage))
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 8.0f, 0.0f, 0.0f)
				[
					SNew(SBox)
					.Visibility_Lambda([this]() { return State == EState::RestartNeeded ? EVisibility::Collapsed : EVisibility::Visible; })
					[
						VrrMenu::ActionButton(FText::Format(LOCTEXT("EnableButton", "Enable {0}"), PluginName),
							LOCTEXT("EnableTip", "Enables AnimToTexture in the project file (.uproject), the same as ticking it in Edit > Plugins. Restart the editor afterwards."),
							[this]() -> FText
							{
								switch (State)
								{
								case EState::Missing: return LOCTEXT("BlockMissing", "AnimToTexture isn't installed with this engine.");
								case EState::LoadFailed: return LOCTEXT("BlockLoaded", "AnimToTexture is already enabled.");
								default: return FText::GetEmpty();
								}
							},
							FOnClicked::CreateSP(this, &SMenu_VATPlugin::OnEnablePlugin))
					]
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 8.0f, 0.0f, 0.0f)
				[
					SNew(SBox)
					.Visibility_Lambda([this]() { return State == EState::RestartNeeded ? EVisibility::Visible : EVisibility::Collapsed; })
					[
						VrrMenu::ActionButton(LOCTEXT("RestartButton", "Restart Editor"),
							LOCTEXT("RestartTip", "Restarts the editor so AnimToTexture loads (asks to save unsaved work first)."),
							[]() { return FText::GetEmpty(); },
							FOnClicked::CreateSP(this, &SMenu_VATPlugin::OnRestartEditor))
					]
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 4.0f, 0.0f, 0.0f)
				[
					SNew(SBox)
					.Visibility_Lambda([this]() { return StatusText.IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible; })
					[
						VrrMenu::StatusLine(&StatusText, &bStatusIsError)
					]
				])
		})
	];
}

FText SMenu_VATPlugin::GetMessage() const
{
	switch (State)
	{
	case EState::RestartNeeded:
		return LOCTEXT("MsgRestart", "AnimToTexture is enabled in the project file. Restart the editor to open the VAT page.");
	case EState::Missing:
		return LOCTEXT("MsgMissing", "AnimToTexture wasn't found in this engine. It ships in Engine/Plugins/Experimental/AnimToTexture; reinstall or add it, then restart the editor.");
	case EState::LoadFailed:
		return LOCTEXT("MsgLoadFailed", "AnimToTexture is enabled, but the TechArtToolboxVAT module didn't load. Rebuild the TechArt Toolbox plugin and check the Output Log.");
	default:
		return LOCTEXT("MsgDisabled", "AnimToTexture isn't enabled in this project. Enable it here (or in Edit > Plugins), then restart the editor.");
	}
}

FReply SMenu_VATPlugin::OnEnablePlugin()
{
	FText Fail;
	bool bOk = IProjectManager::Get().SetPluginEnabled(ITechArtToolboxVATModule::RequiredPlugin, true, Fail);
	if (bOk && IProjectManager::Get().IsCurrentProjectDirty())
	{
		// Checks the .uproject out of source control (or clears read-only) first, as Edit > Plugins does
		FGameProjectGenerationModule::Get().TryMakeProjectFileWriteable(FPaths::GetProjectFilePath());
		bOk = IProjectManager::Get().SaveCurrentProjectToDisk(Fail);
	}

	if (!bOk)
	{
		StatusText = FText::Format(LOCTEXT("EnableFailed", "Couldn't update the project file: {0} Enable AnimToTexture in Edit > Plugins instead."), Fail);
		bStatusIsError = true;
		VrrMenu::Notify(StatusText, VrrMenu::ENotify::Error);
		return FReply::Handled();
	}

	State = EState::RestartNeeded;
	StatusText = LOCTEXT("EnableDone", "AnimToTexture enabled. Restart the editor to load it.");
	bStatusIsError = false;
	return FReply::Handled();
}

FReply SMenu_VATPlugin::OnRestartEditor()
{
	// No extra prompt: the normal editor shutdown still asks to save unsaved assets
	FUnrealEdMisc::Get().RestartEditor(false);
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
