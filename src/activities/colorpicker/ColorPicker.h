#pragma once
#include "Config.h"

#include "gui/PopupWindow.h"
#include "gui/Color.h"

namespace activities::colorpicker
{
	class Track1D;

	enum InputGroupName
	{
		inputGroupRgb,
		inputGroupHsv,
		inputGroupA,
		// inputGroupUser0,
		// inputGroupUser1,
		// inputGroupUser2,
		inputGroupCount, // * Must be the last enum constant.
	};
	enum InputName
	{
		inputRgbR,
		inputRgbG,
		inputRgbB,
		inputHsvH,
		inputHsvS,
		inputHsvV,
		inputAlpA,
		inputCount, // * Must be the last enum constant.
	};

	class ColorPicker : public gui::PopupWindow
	{
		std::array<Track1D *, inputCount> inputTracks = {};
		gui::FColor color = { 0, 0, 0, 0 };

		void Pick(InputName input);

	public:
		ColorPicker();
	};
}
