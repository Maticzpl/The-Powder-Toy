#include "ColorPicker.h"

#include "gui/Appearance.h"
#include "gui/Button.h"
#include "gui/Dragger.h"
#include "gui/Static.h"
#include "gui/Separator.h"
#include "gui/SDLWindow.h"
#include "gui/TrackBar.h"
#include "gui/Texture.h"
#include "prefs/GlobalPrefs.h"
#include "language/Language.h"
#include "activities/game/Game.h"
#include "graphics/Pix.h"
#include "Misc.h"

#include <SDL.h>
#include <iostream>
#include <array>

namespace activities::colorpicker
{
	constexpr auto parentSize = gui::Point{ WINDOWW, WINDOWH };
	constexpr auto windowSize = gui::Point{ 200, 169 };

	class Track1DHandle : public gui::Dragger
	{
		void Draw() const final override;
	};

	using InputBlock = std::array<float, inputCount>;

	struct InputGroup
	{
		gui::FColor (*toColor)(const InputBlock &);
		void (*fromColor)(InputBlock &, const gui::FColor);
	};
	static const std::array<InputGroup, inputGroupCount> inputGroups = {{
		{ // inputGroupRgb
			[](const InputBlock &ib) {
				return gui::FColor{ ib[inputRgbR], ib[inputRgbG], ib[inputRgbB], ib[inputAlpA] };
			},
			[](InputBlock &ib, const gui::FColor color) {
				ib[inputRgbR] = color.r;
				ib[inputRgbG] = color.g;
				ib[inputRgbB] = color.b;
			},
		},
		{ // inputGroupHsv
			[](const InputBlock &ib) {
				int r, g, b;
				HSV_to_RGB(int(roundf(ib[inputHsvH] * 360)), int(roundf(ib[inputHsvS] * 255)), int(roundf(ib[inputHsvV] * 255)), &r, &g, &b);
				return gui::FColor{ r / 255.f, g / 255.f, b / 255.f, ib[inputAlpA] };
			},
			[](InputBlock &ib, const gui::FColor color) {
				int h, s, v;
				RGB_to_HSV(int(roundf(color.r * 255)), int(roundf(color.g * 255)), int(roundf(color.b * 255)), &h, &s, &v);
				ib[inputHsvH] = h / 360.f;
				ib[inputHsvS] = s / 255.f;
				ib[inputHsvV] = v / 255.f;
			},
		},
		{ // inputGroupA
			[](const InputBlock &ib) {
				return gui::FColor{ ib[inputRgbR], ib[inputRgbG], ib[inputRgbB], ib[inputAlpA] };
			},
			[](InputBlock &ib, const gui::FColor color) {
				ib[inputAlpA] = color.a;
			},
		},
	}};

	struct Input
	{
		int low;
		int high;
		bool inclusive;
		InputGroupName group;
	};
	static const std::array<Input, inputCount> inputs = {{
		{ 0, 255,  true, inputGroupRgb },
		{ 0, 255,  true, inputGroupRgb },
		{ 0, 255,  true, inputGroupRgb },
		{ 0, 360, false, inputGroupHsv },
		{ 0, 255,  true, inputGroupHsv },
		{ 0, 255,  true, inputGroupHsv },
		{ 0, 255,  true, inputGroupA   },
	}};

	class Track1D : public gui::TrackBar
	{
		InputName input;
		gui::Texture *gradient = nullptr;

	public:
		Track1D(InputName newInput) : input(newInput)
		{
			gradient = EmplaceChild<gui::Texture>().get();
			Handle(std::make_shared<Track1DHandle>());
			ValueLow({ 0, 0 });
			ValueHigh({ 1, 0 });
			Inclusive(inputs[input].inclusive);
		}

		void Draw() const final override
		{
			auto abs = AbsolutePosition();
			auto size = Size();
			auto &g = gui::SDLWindow::Ref();
			g.DrawRectOutline({ abs, size }, { 0x80, 0x80, 0x80, 0xFF });
			g.DrawTexture(*gradient, { abs + gui::Point{ 1, 1 }, size - gui::Point{ 2, 2 } });
		}

		void UpdateGradient(gui::FColor current)
		{
			auto *toColor = inputGroups[inputs[input].group].toColor;
			auto image = gui::Image::FromSize(Size() - gui::Point{ 2, 2 });
			InputBlock ib;
			for (auto &ig : inputGroups)
			{
				ig.fromColor(ib, current);
			}
			auto scale = (image.Size().x - (Inclusive() ? 1 : 0)) / (ValueHigh().x - ValueLow().x);
			for (auto x = 0; x < image.Size().x; ++x)
			{
				auto blend = [](gui::FColor src, gui::FColor dest) {
					src.r = src.a * src.r + (1.f - src.a) * dest.r;
					src.g = src.a * src.g + (1.f - src.a) * dest.g;
					src.b = src.a * src.b + (1.f - src.a) * dest.b;
					return src;
				};
				ib[input] = ValueLow().x + float(x) / scale;
				auto fromIb = toColor(ib);
				auto light = blend(fromIb, {   1.f,   1.f,   1.f });
				auto dark  = blend(fromIb, { 0.75f, 0.75f, 0.75f });
				auto lightU32 = PixRGB(int(roundf(light.r * 255.f)), int(roundf(light.g * 255.f)), int(roundf(light.b * 255.f)));
				auto darkU32  = PixRGB(int(roundf( dark.r * 255.f)), int(roundf( dark.g * 255.f)), int(roundf( dark.b * 255.f)));
				for (auto y = 0; y < image.Size().y; ++y)
				{
					image(x, y) = (((x / 3) + (y / 3)) & 1) ? lightU32 : darkU32;
				}
			}
			gradient->ImageData(image);
		}

		gui::Color ColorAt(int x) const
		{
			auto color = gradient->ImageData()(x, 0);
			return { PixR(color), PixR(color), PixB(color) };
		}
	};

	void Track1DHandle::Draw() const
	{
		auto *track = static_cast<Track1D *>(Parent());
		auto abs = AbsolutePosition();
		auto size = track->Size();
		auto &g = gui::SDLWindow::Ref();
		g.DrawLine(
			abs,
			abs + gui::Point{ 0, size.y - 3 },
			gui::Appearance::darkColor(track->ColorAt(Position().x - 1)) ?
			gui::Color{ 0xC0, 0xC0, 0xC0, 0xFF } :
			gui::Color{ 0x40, 0x40, 0x40, 0xFF }
		);
		for (auto i = 0; i <= 2; ++i)
		{
			g.DrawLine(abs + gui::Point{ -i,         -1 - i }, abs + gui::Point{ i,         -1 - i }, { 0xFF, 0xFF, 0xFF, 0xFF });
			g.DrawLine(abs + gui::Point{ -i, size.y - 2 + i }, abs + gui::Point{ i, size.y - 2 + i }, { 0xFF, 0xFF, 0xFF, 0xFF });
		}
	}

	ColorPicker::ColorPicker()
	{
		{
			auto title = EmplaceChild<gui::Static>();
			title->Position({ 6, 5 });
			title->Size({ windowSize.x - 20, 14 });
			title->Text("DEFAULT_LS_COLORPICKER_TITLE"_Ls());
			title->TextColor(gui::Appearance::informationTitle);

			auto sep = EmplaceChild<gui::Separator>();
			sep->Position({ 1, 22 });
			sep->Size({ windowSize.x - 2, 1 });

			auto ok = EmplaceChild<gui::Button>();
			ok->Position({ 0, windowSize.y - 17 });
			ok->Size({ windowSize.x, 17 });
			ok->Text(String::Build(gui::CommonColorString(gui::commonYellow), "DEFAULT_LS_COLORPICKER_OK"_Ls()));
			ok->Click([this]() {
				auto decoColor = gui::Color{
					int(roundf(color.r * 255)),
					int(roundf(color.g * 255)),
					int(roundf(color.b * 255)),
					int(roundf(color.a * 255)),
				};
				game::Game::Ref().DecoColor(PixRGBA(decoColor.r, decoColor.g, decoColor.b, decoColor.a));
				auto &gpref = prefs::GlobalPrefs::Ref();
				prefs::Prefs::DeferWrite dw(gpref);
				gpref.Set("Decoration.Red",   decoColor.r);
				gpref.Set("Decoration.Green", decoColor.g);
				gpref.Set("Decoration.Blue",  decoColor.b);
				gpref.Set("Decoration.Alpha", decoColor.a);
				Quit();
			});
			Okay([ok]() {
				ok->TriggerClick();
			});
		}

		auto track1D = [this](InputName input, gui::Point pos) {
			auto *tb = EmplaceChild<Track1D>(input).get();
			tb->Position(pos);
			tb->Size({ windowSize.x - 16, 11 });
			tb->HandleSize({ 1, 1 });
			tb->HandleRect({ { 1, 1 }, { windowSize.x - 18, 1 } });
			tb->Change([this, input]() {
				Pick(input);
			});
			inputTracks[input] = tb;
		};
		track1D(inputRgbR, { 8, 31 + 0 * 17 }); // TODO: Position.
		track1D(inputRgbG, { 8, 31 + 1 * 17 });
		track1D(inputRgbB, { 8, 31 + 2 * 17 });
		track1D(inputHsvH, { 8, 31 + 3 * 17 });
		track1D(inputHsvS, { 8, 31 + 4 * 17 });
		track1D(inputHsvV, { 8, 31 + 5 * 17 });
		track1D(inputAlpA, { 8, 31 + 6 * 17 });

		Size(windowSize);
		Position((parentSize - windowSize) / 2);

		Pick(inputCount); // * This invalid input index acts like a "take colour from game::Game" command.
	}

	void ColorPicker::Pick(InputName input)
	{
		auto group = inputGroupCount;
		if (input < inputCount)
		{
			group = inputs[input].group;
		}
		else
		{
			auto deco = game::Game::Ref().DecoColor();
			color = {
				PixR(deco) / 255.f,
				PixG(deco) / 255.f,
				PixB(deco) / 255.f,
				PixA(deco) / 255.f,
			};
		}
		InputBlock ib;
		for (auto i = 0; i < inputGroupCount; ++i)
		{
			inputGroups[i].fromColor(ib, color);
		}
		for (auto i = 0; i < inputCount; ++i)
		{
			if (inputs[i].group == group)
			{
				ib[i] = inputTracks[i]->Value().x;
			}
		}
		if (input < inputCount)
		{
			color = inputGroups[group].toColor(ib);
		}
		for (auto i = 0; i < inputCount; ++i)
		{
			if (inputs[i].group != group)
			{
				inputTracks[i]->Value({ ib[i], 0 });
			}
			if (i != input)
			{
				inputTracks[i]->UpdateGradient(color);
			}
		}
	}
}
