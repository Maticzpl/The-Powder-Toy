#include "Save.h"

#include "Metrics.h"
#include "gui/Appearance.h"
#include "gui/Button.h"
#include "gui/ImageButton.h"
#include "gui/TextBox.h"
#include "gui/Label.h"
#include "gui/Static.h"
#include "gui/Spinner.h"
#include "gui/Separator.h"
#include "gui/Texture.h"
#include "graphics/FontData.h"
#include "gui/SDLWindow.h"
#include "language/Language.h"
#include "simulation/GameSave.h"
#include "graphics/ThumbnailRenderer.h"
#include "simulation/ElementGraphics.h"

namespace activities::save
{
	Save::Save(String name, std::shared_ptr<const GameSave> saveData, gui::Point windowSize)
	{
		title = EmplaceChild<gui::Static>().get();
		title->Position({ 6, 5 });
		title->Size({ windowSize.x - 20, 14 });
		title->TextColor(gui::Appearance::informationTitle);

		auto *sep = EmplaceChild<gui::Separator>().get();
		sep->Position({ 1, 22 });
		sep->Size({ windowSize.x - 2, 1 });

		saveButton = EmplaceChild<gui::Button>().get();
		saveButton->Text(String::Build(gui::CommonColorString(gui::commonYellow), "DEFAULT_LS_SAVE_SAVE"_Ls()));
		saveButton->Click([this]() {
			auto &title = nameBox->Text();
			if (!title.size())
			{
				return;
			}
			// statusText.clear();
			// Update();
		});
		Okay([saveButton = saveButton]() {
			saveButton->TriggerClick();
		});

		cancelButton = EmplaceChild<gui::Button>().get();
		cancelButton->Text("DEFAULT_LS_SAVE_CANCEL"_Ls());
		cancelButton->Click([this]() {
			Quit();
		});

		thumbnail = EmplaceChild<gui::ImageButton>().get();
		thumbnail->Position({ 8, 31 });
		thumbnail->Size(thumbnailSize + gui::Point{ 2, 2 });
		thumbnail->BackgroundRect({ { 1, 1 }, thumbnailSize });
		thumbnail->DrawBorder(true);
		thumbnail->ToFrontUnderMouse(false);

		thumbnailSpinner = EmplaceChild<gui::Spinner>().get();
		thumbnailSpinner->Position((thumbnailSize - spinnerSize) / 2 + thumbnail->Position());
		thumbnailSpinner->Size(spinnerSize);

		try
		{
			gui::Point sizeB;
			saveData->BlockSizeAfterExtract(0, 0, 0, sizeB.x, sizeB.y);
			saveRender = graphics::ThumbnailRendererTask::Create({
				std::move(saveData),
				false,
				true,
				RENDER_BASC | RENDER_FIRE | RENDER_SPRK | RENDER_EFFE,
				0,
				COLOUR_DEFAULT,
				0,
				{ 0, 0 },
				{ { 0, 0 }, sizeB },
			});
			saveRender->Start(saveRender);
		}
		catch (const ParseException &e)
		{
			// * TODO-REDO_UI: Do something with this.
		}

		nameBox = EmplaceChild<gui::TextBox>().get();
		nameBox->PlaceHolder(String::Build("[", "DEFAULT_LS_SAVE_TITLE"_Ls(), "]"));
		nameBox->Position({ 119, 31 });
		nameBox->Size({ windowSize.x - 127, 17 });
		nameBox->Text(name);
		nameBox->SelectAll();

		background = EmplaceChild<gui::Texture>().get();
		background->Blend(gui::Texture::blendAdd);

		Size(windowSize);
		Position((parentSize - windowSize) / 2);
		backgroundRect.pos = { Position().x - 104, 20 };
	}

	Save::~Save()
	{
	}

	void Save::UpdateTasks()
	{
		if (saveRender && saveRender->complete)
		{
			if (!saveRender->status)
			{
				// * TODO-REDO_UI: Make it so that this is guaranteed to not happen.
				throw std::runtime_error("rendering paste thumbnail failed");
			}
			saveRender->output.Size(thumbnailSize, gui::Image::resizeModeLinear);
			thumbnail->Background()->ImageData(saveRender->output);
			thumbnailSpinner->Visible(false);
			saveRender.reset();
		}
	}

	void Save::Tick()
	{
		PopupWindow::Tick();
		UpdateTasks();
	}

	void Save::Draw() const
	{
		if (background->Handle())
		{
			gui::SDLWindow::Ref().DrawTexture(*background, backgroundRect);
		}
		PopupWindow::Draw();
	}
}
