#pragma once
#include "Config.h"

#include "Static.h"

#include <functional>

namespace gui
{
	class Button : public Static
	{
	public:
		using ClickCallback = std::function<void ()>;
		using AltClickCallback = std::function<void ()>;
		enum ActiveTextMode
		{
			activeTextInverted,
			activeTextDarkened,
			activeTextNone,
			activeTextMax, // * Must be the last enum constant.
		};
		enum DisabledTextMode
		{
			disabledTextDarkened,
			disabledTextNone,
			disabledTextMax, // * Must be the last enum constant.
		};

	private:
		bool stuck = false;
		bool drawBody = true;
		bool drawBorder = true;
		ClickCallback click;
		AltClickCallback altClick;
		Color bodyColor = { 0, 0, 0, 0 };
		ActiveTextMode activeTextMode = activeTextInverted;
		DisabledTextMode disabledTextMode = disabledTextDarkened;

		void SetToolTip();
		void ResetToolTip();
		void UpdateToolTip();

	public:
		Button();

		void Draw() const override;
		bool MouseDown(Point current, int button) final override;
		void MouseClick(Point current, int button, int clicks) override;
		bool KeyPress(int sym, int scan, bool repeat, bool shift, bool ctrl, bool alt) final override;
		bool KeyRelease(int sym, int scan, bool repeat, bool shift, bool ctrl, bool alt) final override;

		void Click(ClickCallback newClick)
		{
			click = newClick;
		}
		ClickCallback Click() const
		{
			return click;
		}

		void AltClick(AltClickCallback newAltClick)
		{
			altClick = newAltClick;
		}
		AltClickCallback AltClick() const
		{
			return altClick;
		}

		void Stuck(bool newStuck)
		{
			stuck = newStuck;
		}
		bool Stuck() const
		{
			return stuck;
		}

		void BodyColor(Color newBodyColor)
		{
			bodyColor = newBodyColor;
		}
		Color BodyColor() const
		{
			return bodyColor;
		}

		void ActiveText(ActiveTextMode newActiveTextMode)
		{
			if (int(newActiveTextMode) >= 0 && int(newActiveTextMode) < int(activeTextMax))
			{
				activeTextMode = newActiveTextMode;
			}
		}
		ActiveTextMode ActiveText() const
		{
			return activeTextMode;
		}

		void DisabledText(DisabledTextMode newDisabledTextMode)
		{
			if (int(newDisabledTextMode) >= 0 && int(newDisabledTextMode) < int(disabledTextMax))
			{
				disabledTextMode = newDisabledTextMode;
			}
		}
		DisabledTextMode DisabledText() const
		{
			return disabledTextMode;
		}

		void DrawBody(bool newDrawBody)
		{
			drawBody = newDrawBody;
		}
		bool DrawBody() const
		{
			return drawBody;
		}

		void DrawBorder(bool newDrawBorder)
		{
			drawBorder = newDrawBorder;
		}
		bool DrawBorder() const
		{
			return drawBorder;
		}

		virtual void HandleClick(int button);
		void TriggerClick();
		void TriggerAltClick();
	};
}
