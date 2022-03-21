#include "ScrollPanel.h"

#include "SDLWindow.h"

#include <SDL.h>
#include <iostream>

namespace gui
{
	constexpr auto trackDepth         = 6;
	constexpr auto draggerMinSize     = 20;
	constexpr auto scrollBarPopupTime = 0.1f;
	constexpr auto momentumMin        = 30.f;
	constexpr auto momentumMax        = 420.f;
	constexpr auto momentumHalflife   = 0.6f;
	constexpr auto momentumStep       = 120.f;
	constexpr auto plainStep          = 20;

	ScrollPanel::ScrollPanel()
	{
		scrollBarPopup.LowerLimit(0);
		scrollBarPopup.UpperLimit(1);
		scrollBarPopup.Slope(1 / scrollBarPopupTime);
		for (auto dim = 0; dim < 2; ++dim)
		{
			scroll[dim].anim.SmallThreshold(momentumMin);
			scroll[dim].anim.HalfLife(momentumHalflife);
			scroll[dim].anim.Limit(momentumMax);
		}
		ClipChildren(true);
		interior = EmplaceChild<Component>().get();
	}

	void ScrollPanel::Tick()
	{
		scrollBarPopup.UpDown(UnderMouse());
		auto size = Size();
		auto offset = interior->Position();
		auto isize = interior->Size();
		auto sizeDiff = size - isize;
		bool offsetChanged = false;
		bool enableChanged = false;
		auto momentumScroll = SDLWindow::Ref().MomentumScroll();
		for (auto dim = 0; dim < 2; ++dim)
		{
			auto oldEnable = scroll[dim].enabled;
			scroll[dim].enabled = sizeDiff[dim] < 0;
			if (scroll[dim].enabled != oldEnable)
			{
				enableChanged = true;
			}
			scroll[dim].draggerSize = 0;
			scroll[dim].draggerPosition = 0;
			if (momentumScroll)
			{
				scroll[dim].anim.CancelIfSmall();
				auto effective = scroll[dim].anim.EffectivePosition();
				if (offset[dim] != effective)
				{
					offset[dim] = effective;
					offsetChanged = true;
				}
			}
			else
			{
				scroll[dim].anim.Cancel(offset[dim]);
			}
			auto lowTarget = sizeDiff[dim];
			if (!scroll[dim].alignHigh)
			{
				lowTarget = std::min(lowTarget, 0);
			}
			if (offset[dim] < lowTarget)
			{
				offset[dim] = lowTarget;
				scroll[dim].anim.Cancel(offset[dim]);
				offsetChanged = true;
			}
			auto highTarget = 0;
			if (scroll[dim].alignHigh)
			{
				highTarget = std::max(highTarget, sizeDiff[dim]);
			}
			if (offset[dim] > highTarget)
			{
				offset[dim] = highTarget;
				scroll[dim].anim.Cancel(offset[dim]);
				offsetChanged = true;
			}
			if (scroll[dim].enabled && isize[dim] != 0)
			{
				scroll[dim].draggerSize = size[dim] * size[dim] / isize[dim];
				if (scroll[dim].draggerSize < draggerMinSize)
				{
					scroll[dim].draggerSize = draggerMinSize;
				}
				scroll[dim].draggerPosition = offset[dim] * (size[dim] - scroll[dim].draggerSize) / sizeDiff[dim];
			}
		}
		if (enableChanged)
		{
			UpdateForwardRect();
		}
		if (offsetChanged)
		{
			interior->Position(offset);
		}
	}

	void ScrollPanel::DrawAfterChildren() const
	{
		auto effectiveTrackDepth = int(scrollBarPopup.Current() * trackDepth);
		auto abs = AbsolutePosition();
		auto size = Size();
		auto &g = SDLWindow::Ref();
		auto cTrack   = Color{ 125, 125, 125, 100 };
		auto cDragger = Color{ 255, 255, 255, 255 };
		auto shaveOffX = (scroll[0].enabled && scroll[1].enabled) ? effectiveTrackDepth : 0;
		if (scrollBarVisible && scroll[0].enabled)
		{
			g.DrawRect({ abs + Point{ 0, size.y - effectiveTrackDepth }, { size.x - shaveOffX, effectiveTrackDepth } }, cTrack );
			g.DrawRect({ abs + Point{ scroll[0].draggerPosition, size.y - effectiveTrackDepth }, { scroll[0].draggerSize, effectiveTrackDepth } }, cDragger );
		}
		if (scrollBarVisible && scroll[1].enabled)
		{
			g.DrawRect({ abs + Point{ size.x - effectiveTrackDepth, 0 }, { effectiveTrackDepth,             size.y } }, cTrack );
			g.DrawRect({ abs + Point{ size.x - effectiveTrackDepth, scroll[1].draggerPosition }, { effectiveTrackDepth, scroll[1].draggerSize } }, cDragger );
		}
		Component::DrawAfterChildren();
	}

	bool ScrollPanel::MouseWheel(Point current, int distance, int direction)
	{
		auto size = Size();
		auto offset = interior->Position();
		auto isize = interior->Size();
		auto sizeDiff = size - isize;
		if (direction == 1 && SDLWindow::Ref().ModShift())
		{
			direction = 0;
		}
		if (direction == 0 || direction == 1)
		{
			auto wantIncreaseOffset = distance > 0;
			auto wantDecreaseOffset = distance < 0;
			auto canIncreaseOffset = offset[direction] < 0;
			auto canDecreaseOffset = offset[direction] > sizeDiff[direction];
			if (!canDecreaseOffset && wantDecreaseOffset && overScroll)
			{
				overScroll(direction);
			}
			if (!canIncreaseOffset && wantIncreaseOffset && underScroll)
			{
				underScroll(direction);
			}
			if ((canDecreaseOffset && wantDecreaseOffset) || (canIncreaseOffset && wantIncreaseOffset))
			{
				if (SDLWindow::Ref().MomentumScroll())
				{
					scroll[direction].anim.Accelerate(distance * momentumStep);
				}
				else
				{
					offset[direction] += distance * plainStep;
					interior->Position(offset);
				}
				return true;
			}
		}
		return false;
	}

	void ScrollPanel::MouseDragMove(Point current, int button)
	{
		if (scrollBarVisible && button == SDL_BUTTON_LEFT)
		{
			auto offset = interior->Position();
			auto local = current - AbsolutePosition();
			for (auto dim = 0; dim < 2; ++dim)
			{
				if (scroll[dim].dragActive)
				{
					auto size = Size();
					auto isize = interior->Size();
					auto sizeDiff = size - isize;
					scroll[dim].draggerPosition = local[dim] - scroll[dim].dragInitialMouse + scroll[dim].dragInitialDragger;
					offset[dim] = scroll[dim].draggerPosition * sizeDiff[dim] / (size[dim] - scroll[dim].draggerSize);
					scroll[dim].anim.Cancel(offset[dim]);
				}
			}
			interior->Position(offset);
		}
	}

	void ScrollPanel::MouseDragBegin(Point current, int button)
	{
		if (scrollBarVisible && button == SDL_BUTTON_LEFT)
		{
			auto offset = interior->Position();
			auto local = current - AbsolutePosition();
			for (auto rev = 0; rev < 2; ++rev)
			{
				auto dim = 1 - rev;
				if (scroll[dim].enabled && local[rev] > Size()[rev] - trackDepth)
				{
					scroll[dim].dragActive = true;
					if (scroll[dim].draggerPosition + scroll[dim].draggerSize <= local[dim] || scroll[dim].draggerPosition > local[dim])
					{
						scroll[dim].draggerPosition = local[dim] - scroll[dim].draggerSize / 2;
					}
					auto size = Size();
					auto isize = interior->Size();
					auto sizeDiff = size - isize;
					scroll[dim].dragInitialDragger = scroll[dim].draggerPosition;
					scroll[dim].dragInitialMouse = local[dim];
					offset[dim] = scroll[dim].draggerPosition * sizeDiff[dim] / (size[dim] - scroll[dim].draggerSize);
					scroll[dim].anim.Cancel(offset[dim]);
					CaptureMouse(true);
					break; // * Note how the loop header and this break statement prioritises vertical scrolling over horizontal.
				}
			}
			interior->Position(offset);
		}
	}

	void ScrollPanel::MouseDragEnd(Point current, int button)
	{
		if (scrollBarVisible && button == SDL_BUTTON_LEFT)
		{
			scroll[0].dragActive = false;
			scroll[1].dragActive = false;
			CaptureMouse(false);
		}
	}

	void ScrollPanel::ScrollBarVisible(bool newScrollBarVisible)
	{
		scrollBarVisible = newScrollBarVisible;
		UpdateForwardRect();
	}

	void ScrollPanel::UpdateForwardRect()
	{
		MouseForwardRect({ { 0, 0 }, Size() - Point{ scrollBarVisible && scroll[0].enabled ? trackDepth : 0, scrollBarVisible && scroll[1].enabled ? trackDepth : 0 } });
	}

	void ScrollPanel::Size(Point newSize)
	{
		Component::Size(newSize);
		UpdateForwardRect();
	}

	void ScrollPanel::Offset(Point offset)
	{
		interior->Position(offset);
		for (auto dim = 0; dim < 2; ++dim)
		{
			scroll[dim].anim.Cancel(offset[dim]);
		}
	}
}
