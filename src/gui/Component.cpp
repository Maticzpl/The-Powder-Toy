#include "Component.h"

#include "Rect.h"
#include "SDLWindow.h"

#include <algorithm>
#include <cassert>
#include <SDL.h>

// * TODO-REDO_UI: tab-based focus cycling

static_assert(SDL_BUTTON_LEFT   == 1, "draggingButton will not work as expected");
static_assert(SDL_BUTTON_MIDDLE == 2, "draggingButton will not work as expected");
static_assert(SDL_BUTTON_RIGHT  == 3, "draggingButton will not work as expected");
static_assert(SDL_BUTTON_X1     == 4, "draggingButton will not work as expected");
static_assert(SDL_BUTTON_X2     == 5, "draggingButton will not work as expected");

namespace gui
{
	Component::~Component()
	{
		Children([this](Component *child) -> bool {
			RemoveChild(child);
			return false;
		});
	}

	Component::DeferRemovals::DeferRemovals(Component &component) : component(component)
	{
		component.deferRemovalsDepth += 1;
	}

	Component::DeferRemovals::~DeferRemovals()
	{
		component.deferRemovalsDepth -= 1;
		if (!component.deferRemovalsDepth && component.hasDeferredRemovals)
		{
			component.hasDeferredRemovals = false;
			for (auto it = component.children.begin(); it != component.children.end(); )
			{
				if (it->get())
				{
					++it;
				}
				else
				{
					it = component.children.erase(it);
				}
			}
		}
	}

	template<class It, class VisitCancel>
	bool Component::ChildrenUnordered(It begin, It end, VisitCancel visitCancel)
	{
		DeferRemovals dr(*this);
		for (auto it = begin; it != end; ++it)
		{
			if (it->get() && visitCancel(it->get()))
			{
				return true;
			}
		}
		return false;
	}

	template<class It, class VisitCancel>
	bool Component::ChildrenOrdered(It begin, It end, VisitCancel visitCancel) const
	{
		for (auto it = begin; it != end; ++it)
		{
			if (*it && visitCancel(*it))
			{
				return true;
			}
		}
		return false;
	}

	template<class VisitCancel>
	bool Component::Children(VisitCancel visitCancel)
	{
		return ChildrenUnordered(children.begin(), children.end(), visitCancel);
	}

	template<class VisitCancel>
	bool Component::ChildrenForward(VisitCancel visitCancel) const
	{
		return ChildrenOrdered(childrenOrdered.begin(), childrenOrdered.end(), visitCancel);
	}

	template<class VisitCancel>
	bool Component::ChildrenReverse(VisitCancel visitCancel) const
	{
		return ChildrenOrdered(childrenOrdered.rbegin(), childrenOrdered.rend(), visitCancel);
	}

	Component *Component::FindChildUnderMouse(Point pos) const
	{
		Component *found = nullptr;
		if (mouseForwardRect.Offset(absolutePosition).Contains(pos))
		{
			ChildrenForward([&found, pos](Component *child) {
				if (child->visible && (child->stealsMouse || Rect{ child->absolutePosition, child->size }.Contains(pos)))
				{
					found = child;
					return true;
				}
				return false;
			});
		}
		return found;
	}

	void Component::UpdateChildUnderMouse(const Event *higherEv)
	{
		if (!underMouse)
		{
			return;
		}
		auto *prevChildUnderMouse = childUnderMouse;
		childUnderMouse = childWithMouseCapture;
		if (!childUnderMouse)
		{
			childUnderMouse = FindChildUnderMouse(mousePosition);
		}
		if (prevChildUnderMouse == childUnderMouse)
		{
			if (childUnderMouse)
			{
				if (higherEv)
				{
					childUnderMouse->HandleEvent(*higherEv);
				}
				else
				{
					childUnderMouse->UpdateChildUnderMouse(nullptr);
				}
			}
		}
		else
		{
			if (prevChildUnderMouse)
			{
				Event ev;
				ev.type = Event::MOUSELEAVE;
				prevChildUnderMouse->HandleEvent(ev);
			}
			if (childUnderMouse)
			{
				Event ev;
				ev.type = Event::MOUSEENTER;
				ev.mouse.current = mousePosition;
				childUnderMouse->HandleEvent(ev);
			}
		}
		if (childUnderMouse && childUnderMouse->enabled && childUnderMouse->toFrontUnderMouse)
		{
			ChildToFrontEnsureOrder(childUnderMouse);
		}
	}

	void Component::SetChildWithFocus(Component *newChildWithFocus)
	{
		if (childWithFocus == newChildWithFocus)
		{
			return;
		}
		if (withFocus && childWithFocus)
		{
			Event ev;
			ev.type = Event::FOCUSLOSE;
			childWithFocus->HandleEvent(ev);
		}
		childWithFocus = newChildWithFocus;
		if (withFocus && childWithFocus)
		{
			Event ev;
			ev.type = Event::FOCUSGAIN;
			childWithFocus->HandleEvent(ev);
		}
	}

	void Component::SetChildWithMouseCapture(Component *newChildWithMouseCapture)
	{
		if (childWithMouseCapture == newChildWithMouseCapture)
		{
			return;
		}
		childWithMouseCapture = newChildWithMouseCapture;
		UpdateChildUnderMouse(nullptr);
	}

	bool Component::HandleEvent(const Event &ev)
	{
		switch (ev.type)
		{
		case Event::TICK:
			Tick();
			Children([&ev](Component *child) {
				auto ptr = *child->childrenIt;
				ptr->HandleEvent(ev);
				return false;
			});
			return false;

		case Event::QUIT:
			Children([&ev](Component *child) {
				auto ptr = *child->childrenIt;
				ptr->HandleEvent(ev);
				return false;
			});
			Quit();
			return false;
		
		case Event::RENDERERUP:
			renderer = ev.renderer.renderer;
			{
				SDLWindow::ForbidMethods fm(SDLWindow::forbidTreeOp);
				RendererUp();
			}
			Children([&ev](Component *child) {
				child->HandleEvent(ev);
				return false;
			});
			return false;
		
		case Event::RENDERERDOWN:
			Children([&ev](Component *child) {
				child->HandleEvent(ev);
				return false;
			});
			{
				SDLWindow::ForbidMethods fm(SDLWindow::forbidTreeOp);
				RendererDown();
			}
			renderer = nullptr;
			return false;

		case Event::MOUSEENTER:
			SDLWindow::Ref().ShouldUpdateHoverCursor();
			mousePosition = ev.mouse.current;
			underMouse = true;
			{
				SDLWindow::ForbidMethods fm(SDLWindow::forbidGenerated);
				MouseEnter(ev.mouse.current);
			}
			childUnderMouse = FindChildUnderMouse(ev.mouse.current);
			if (childUnderMouse)
			{
				childUnderMouse->HandleEvent(ev);
				return true;
			}
			return false;
		
		case Event::MOUSELEAVE:
			SDLWindow::Ref().ShouldUpdateHoverCursor();
			{
				SDLWindow::ForbidMethods fm(SDLWindow::forbidGenerated);
				MouseLeave();
			}
			if (childUnderMouse)
			{
				childUnderMouse->HandleEvent(ev);	
				childUnderMouse = nullptr;
			}
			for (int button = 1; button <= 5; ++button)
			{
				if (draggingButton & (1 << button))
				{
					MouseDragEnd(mousePosition, button);
				}
			}
			draggingButton = 0;
			underMouse = false;
			return false;
		
		case Event::MOUSEMOVE:
			{
				SDLWindow::ForbidMethods fm(SDLWindow::forbidGenerated);
				auto prev = mousePosition;
				mousePosition = ev.mouse.current;
				MouseMove(mousePosition, mousePosition - prev);
			}
			for (int button = 1; button <= 5; ++button)
			{
				if (draggingButton & (1 << button))
				{
					MouseDragMove(mousePosition, button);
				}
			}
			UpdateChildUnderMouse(&ev);
			return false;
		
		case Event::MOUSEDOWN:
		case Event::MOUSEUP:
		case Event::MOUSEWHEEL:
			if (childUnderMouse)
			{
				auto ptr = *childUnderMouse->childrenIt;
				if (ptr->enabled && ptr->HandleEvent(ev))
				{
					return true;
				}
			}
			else
			{
				SetChildWithFocus(nullptr);
			}

			switch (ev.type)
			{
			case Event::MOUSEDOWN:
				{
					bool eat = MouseDown(mousePosition, ev.mouse.button);
					if (!(draggingButton & (1 << ev.mouse.button)))
					{
						draggingButton |= 1 << ev.mouse.button;
						Focus();
						MouseDragBegin(mousePosition, ev.mouse.button);
					}
					return eat;
				}

			case Event::MOUSEUP:
				if (draggingButton & (1 << ev.mouse.button))
				{
					draggingButton &= ~(1 << ev.mouse.button);
					MouseDragEnd(mousePosition, ev.mouse.button);
					MouseClick(mousePosition, ev.mouse.button, ev.mouse.clicks);
				}
				return MouseUp(mousePosition, ev.mouse.button);

			case Event::MOUSEWHEEL:
				Focus();
				return MouseWheel(mousePosition, ev.mouse.wheel, ev.mouse.button);

			default:
				break;
			}
			return false;

		case Event::FOCUSGAIN:
			withFocus = true;
			{
				SDLWindow::ForbidMethods fm(SDLWindow::forbidGenerated);
				FocusGain();
			}
			if (childWithFocus)
			{
				childWithFocus->HandleEvent(ev);
			}
			return false;

		case Event::FOCUSLOSE:
			if (childWithFocus)
			{
				childWithFocus->HandleEvent(ev);
			}
			withFocus = false;
			{
				SDLWindow::ForbidMethods fm(SDLWindow::forbidGenerated);
				FocusLose();
			}
			return false;

		case Event::KEYPRESS:
		case Event::KEYRELEASE:
		case Event::TEXTINPUT:
		case Event::TEXTEDITING:
			if (childWithFocus)
			{
				auto ptr = *childWithFocus->childrenIt;
				if (ptr->enabled && ptr->HandleEvent(ev))
				{
					return true;
				}
			}

			switch (ev.type)
			{
			case Event::KEYPRESS:
				return KeyPress(ev.key.sym, ev.key.scan, ev.key.repeat, ev.key.shift, ev.key.ctrl, ev.key.alt);

			case Event::KEYRELEASE:
				return KeyRelease(ev.key.sym, ev.key.scan, ev.key.repeat, ev.key.shift, ev.key.ctrl, ev.key.alt);

			case Event::TEXTINPUT:
				return TextInput(*ev.text.data);

			case Event::TEXTEDITING:
				TextEditing(*ev.text.data);
				break;

			default:
				break;
			}
			return false;
		
		case Event::FILEDROP:
			if (!underMouse || !childUnderMouse)
			{
				return false;
			}
			{
				auto ptr = *childUnderMouse->childrenIt;
				if (ptr->HandleEvent(ev))
				{
					return true;
				}
			}
			return FileDrop(*ev.filedrop.path);

		default:
			break;
		}

		return false;
	}

	bool Component::HandleEvent(const Event &ev) const
	{
		switch (ev.type)
		{
		case Event::DRAW:
			{
				Draw();
				Rect prevClip;
				auto &sdlw = SDLWindow::Ref();
				if (clipChildren)
				{
					prevClip = sdlw.ClipRect();
					sdlw.ClipRect(prevClip.Intersect(clipChildrenRect.Offset(absolutePosition)));
				}
				auto outerRect = Rect{ absolutePosition, size };
				ChildrenReverse([&ev, outerRect](const Component *child) {
					if (!child->visible)
					{
						return false;
					}
					auto innerRect = Rect{ child->absolutePosition, child->size };
					auto commonRect = innerRect.Intersect(outerRect);
					if (commonRect.size.x == 0 || commonRect.size.y == 0)
					{
						return false;
					}
					child->HandleEvent(ev);
					return false;
				});
				if (clipChildren)
				{
					sdlw.ClipRect(prevClip);
				}
				DrawAfterChildren();
			}
			return false;

		default:
			break;
		}

		return false;
	}

	bool Component::InsertChild(std::shared_ptr<Component> component)
	{
		SDLWindow::Ref().AssertAllowed(SDLWindow::forbidTreeOp | SDLWindow::forbidGenerated);
		if (!component || component->parent)
		{
			return false;
		}
		children.push_front(component);
		childrenOrdered.push_front(component.get());
		component->parent = this;
		component->childrenIt = children.begin();
		component->childrenOrderedIt = childrenOrdered.begin();
		component->UpdateAbsolutePosition();
		UpdateChildUnderMouse(nullptr);
		if (Renderer())
		{
			Event ev;
			ev.type = Event::RENDERERUP;
			ev.renderer.renderer = Renderer();
			component->HandleEvent(ev);
		}
		return true;
	}
	
	std::shared_ptr<Component> Component::GetChild(Component *component) const
	{
		if (!component || component->parent != this)
		{
			return nullptr;
		}
		return *component->childrenIt;
	}

	bool Component::RemoveChild(Component *component)
	{
		SDLWindow::Ref().AssertAllowed(SDLWindow::forbidTreeOp | SDLWindow::forbidGenerated);
		if (!component || component->parent != this)
		{
			return false;
		}
		if (Renderer())
		{
			Event ev;
			ev.type = Event::RENDERERDOWN;
			ev.renderer.renderer = nullptr;
			component->HandleEvent(ev);
		}
		auto oldIt = component->childrenIt;
		auto oldOrderedIt = component->childrenOrderedIt;
		component->parent = nullptr;
		component->childrenIt = {};
		component->childrenOrderedIt = {};
		std::shared_ptr<Component> keep;
		std::shared_ptr<Component> &old = *oldIt;
		std::swap(keep, old);
		if (deferRemovalsDepth)
		{
			hasDeferredRemovals = true;
		}
		else
		{
			children.erase(oldIt);
		}
		childrenOrdered.erase(oldOrderedIt);
		if (component == childWithFocus)
		{
			SetChildWithFocus(nullptr);
		}
		if (component == childWithMouseCapture)
		{
			childWithMouseCapture = nullptr;
			UpdateChildUnderMouse(nullptr);
		}
		if (component == childUnderMouse)
		{
			UpdateChildUnderMouse(nullptr);
		}
		return true;
	}

	void Component::ChildToFrontEnsureOrder(Component *component)
	{
		if (childrenOrdered.front() != component)
		{
			childrenOrdered.emplace_front();
			auto it = childrenOrdered.begin();
			std::swap(*it, *component->childrenOrderedIt);
			std::swap(it, component->childrenOrderedIt);
			childrenOrdered.erase(it);
		}
	}

	void Component::ChildToBackEnsureOrder(Component *component)
	{
		if (childrenOrdered.back() != component)
		{
			childrenOrdered.emplace_back();
			auto it = childrenOrdered.end();
			--it;
			std::swap(*it, *component->childrenOrderedIt);
			std::swap(it, component->childrenOrderedIt);
			childrenOrdered.erase(it);
		}
	}

	bool Component::ChildToFront(Component *component)
	{
		SDLWindow::Ref().AssertAllowed(SDLWindow::forbidGenerated);
		if (!component || component->parent != this)
		{
			return false;
		}
		ChildToFrontEnsureOrder(component);
		UpdateChildUnderMouse(nullptr);
		return true;
	}

	bool Component::ChildToBack(Component *component)
	{
		SDLWindow::Ref().AssertAllowed(SDLWindow::forbidGenerated);
		if (!component || component->parent != this)
		{
			return false;
		}
		ChildToBackEnsureOrder(component);
		UpdateChildUnderMouse(nullptr);
		return true;
	}

	void Component::UpdateAbsolutePosition()
	{
		absolutePosition = relativePosition;
		if (parent)
		{
			absolutePosition += parent->absolutePosition;
		}
		Children([](Component *child) {
			child->UpdateAbsolutePosition();
			return false;
		});
	}

	void Component::Position(Point newPosition)
	{
		SDLWindow::Ref().AssertAllowed(SDLWindow::forbidGenerated);
		relativePosition = newPosition;
		UpdateAbsolutePosition();
		if (parent)
		{
			parent->UpdateChildUnderMouse(nullptr);
		}
		UpdateChildUnderMouse(nullptr);
	}

	void Component::Size(Point newSize)
	{
		SDLWindow::Ref().AssertAllowed(SDLWindow::forbidGenerated);
		size = newSize;
		ClipChildrenRect({ { 0, 0 }, size });
		MouseForwardRect({ { 0, 0 }, size });
	}

	void Component::MouseForwardRect(Rect newMouseForwardRect)
	{
		SDLWindow::Ref().AssertAllowed(SDLWindow::forbidGenerated);
		mouseForwardRect = newMouseForwardRect;
		UpdateChildUnderMouse(nullptr);
	}

	void Component::ToFrontUnderMouse(bool newToFrontUnderMouse)
	{
		toFrontUnderMouse = newToFrontUnderMouse;
		if (parent)
		{
			parent->UpdateChildUnderMouse(nullptr);
		}
	}

	void Component::Enabled(bool newEnabled)
	{
		SDLWindow::Ref().AssertAllowed(SDLWindow::forbidGenerated);
		if (enabled != newEnabled)
		{
			enabled = newEnabled;
			if (parent)
			{
				if (!enabled)
				{
					parent->ChildToBack(this);
				}
				parent->UpdateChildUnderMouse(nullptr);
			}
		}
	}

	void Component::Visible(bool newVisible)
	{
		SDLWindow::Ref().AssertAllowed(SDLWindow::forbidGenerated);
		bool oldVisible = visible;
		visible = newVisible;
		if (oldVisible != visible && parent)
		{
			parent->UpdateChildUnderMouse(nullptr);
			if (!visible && parent->childWithFocus == this)
			{
				parent->SetChildWithFocus(nullptr);
			}
		}
	}

	void Component::Tick()
	{
	}

	void Component::Draw() const
	{
	}

	void Component::DrawAfterChildren() const
	{
#ifdef DEBUG
		if (SDLWindow::Ref().drawComponentRects)
		{
			SDLWindow::Ref().DrawRectOutline({ absolutePosition, size }, { 255, 0, 0, 128 });
		}
#endif
	}

	void Component::Quit()
	{
	}

	void Component::FocusGain()
	{
	}

	void Component::FocusLose()
	{
	}

	void Component::MouseEnter(Point current)
	{
	}

	void Component::MouseLeave()
	{
	}

	bool Component::MouseMove(Point current, Point delta)
	{
		return false;
	}

	bool Component::MouseDown(Point current, int button)
	{
		return false;
	}

	bool Component::MouseUp(Point current, int button)
	{
		return false;
	}

	void Component::MouseDragMove(Point current, int button)
	{
	}

	void Component::MouseDragBegin(Point current, int button)
	{
	}

	void Component::MouseDragEnd(Point current, int button)
	{
	}

	void Component::MouseClick(Point current, int button, int clicks)
	{
	}

	bool Component::MouseWheel(Point current, int distance, int direction)
	{
		return false;
	}

	bool Component::KeyPress(int sym, int scan, bool repeat, bool shift, bool ctrl, bool alt)
	{
		return false;
	}

	bool Component::KeyRelease(int sym, int scan, bool repeat, bool shift, bool ctrl, bool alt)
	{
		return false;
	}

	bool Component::TextInput(const String &data)
	{
		return false;
	}

	void Component::TextEditing(const String &data)
	{
	}

	bool Component::FileDrop(const String &path)
	{
		return false;
	}
	
	void Component::RendererUp()
	{
	}
	
	void Component::RendererDown()
	{
	}

	void Component::CaptureMouse(bool captureMouse)
	{
		auto curr = this;
		auto up = parent;
		while (up)
		{
			up->SetChildWithMouseCapture(captureMouse ? curr : nullptr);
			curr = up;
			up = up->parent;
		}
	}

	void Component::Focus()
	{
		SDLWindow::Ref().AssertAllowed(SDLWindow::forbidGenerated);
		auto curr = this;
		auto up = parent;
		while (up)
		{
			up->SetChildWithFocus(curr);
			curr = up;
			up = up->parent;
		}
	}

	void Component::HoverCursor(Cursor::CursorType newHoverCursor)
	{
		hoverCursor = newHoverCursor;
		if (underMouse)
		{
			SDLWindow::Ref().ShouldUpdateHoverCursor();
		}
	}
}
