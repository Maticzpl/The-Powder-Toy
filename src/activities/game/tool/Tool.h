#pragma once

#include "gui/Color.h"
#include "gui/Image.h"
#include "gui/Point.h"
#include "gui/Rect.h"
#include "common/String.h"

class Simulation;

namespace activities::game
{
	class ToolButton;
}

namespace activities::game::brush
{
	class Brush;
}

namespace activities::game::tool
{
	class Tool
	{
	public:
		enum ApplicationMode
		{
			applicationDraw,
			applicationLine,
			applicationClick,
			applicationMax, // * Must be the last enum constant.
		};

	private:
		String name;
		String description;
		gui::Color color = { 0, 0, 0, 0 };
		bool menuVisible = false;
		int menuSection = 0;
		int menuPrecedence = 0;
		bool useOffsetLists = false;
		bool allowReplace = false;
		bool canGenerateTexture = false;
		bool canDrawButton = false;
		bool cellAligned = false;
		bool enablesGravityZones = false;
		ApplicationMode application = applicationDraw;

	public:
		virtual ~Tool() = default;

		virtual void Draw(gui::Point pos, gui::Point origin) const
		{
		}

		virtual void Fill(gui::Point pos) const
		{
		}

		virtual void Line(gui::Point from, gui::Point to) const
		{
		}

		virtual void Click(gui::Point pos) const
		{
		}

		virtual void GenerateTexture(gui::Image &image) const
		{
		}

		virtual void DrawButton(const ToolButton *toolButton) const
		{
		}

		virtual bool Select()
		{
			return false;
		}

		void Name(const String &newName)
		{
			name = newName;
		}
		const String &Name() const
		{
			return name;
		}

		void Description(const String &newDescription)
		{
			description = newDescription;
		}
		const String &Description() const
		{
			return description;
		}

		void Color(gui::Color newColor)
		{
			color = newColor;
		}
		gui::Color Color() const
		{
			return color;
		}

		void MenuVisible(bool newMenuVisible)
		{
			menuVisible = newMenuVisible;
		}
		bool MenuVisible() const
		{
			return menuVisible;
		}

		void MenuSection(int newMenuSection)
		{
			menuSection = newMenuSection;
		}
		int MenuSection() const
		{
			return menuSection;
		}

		void MenuPrecedence(int newMenuPrecedence)
		{
			menuPrecedence = newMenuPrecedence;
		}
		int MenuPrecedence() const
		{
			return menuPrecedence;
		}

		void UseOffsetLists(bool newUseOffsetLists)
		{
			useOffsetLists = newUseOffsetLists;
		}
		bool UseOffsetLists() const
		{
			return useOffsetLists;
		}

		void AllowReplace(bool newAllowReplace)
		{
			allowReplace = newAllowReplace;
		}
		bool AllowReplace() const
		{
			return allowReplace;
		}

		void CanGenerateTexture(bool newCanGenerateTexture)
		{
			canGenerateTexture = newCanGenerateTexture;
		}
		bool CanGenerateTexture() const
		{
			return canGenerateTexture;
		}

		void CanDrawButton(bool newCanDrawButton)
		{
			canDrawButton = newCanDrawButton;
		}
		bool CanDrawButton() const
		{
			return canDrawButton;
		}

		void CellAligned(bool newCellAligned)
		{
			cellAligned = newCellAligned;
		}
		bool CellAligned() const
		{
			return cellAligned;
		}

		void EnablesGravityZones(bool newEnablesGravityZones)
		{
			enablesGravityZones = newEnablesGravityZones;
		}
		bool EnablesGravityZones() const
		{
			return enablesGravityZones;
		}

		void Application(ApplicationMode newApplication)
		{
			application = newApplication;
		}
		ApplicationMode Application() const
		{
			return application;
		}
	};
}
