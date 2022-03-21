#pragma once
#include "Config.h"

#include "common/String.h"
#include "Event.h"
#include "Point.h"
#include "Rect.h"
#include "Cursor.h"
#include "ToolTipInfo.h"

#include <list>
#include <memory>
#include <utility>
#include <vector>
#include <type_traits>

struct SDL_Renderer;

namespace gui
{
	class Component
	{
		Point relativePosition = { 0, 0 };
		Point absolutePosition = { 0, 0 };
		Point size = { 1, 1 };
		Point mousePosition = { 0, 0 };
		Rect mouseForwardRect = { { 0, 0 }, { 0, 0 } };
		bool enabled = true;
		bool visible = true;
		bool toFrontUnderMouse = true;
		bool clipChildren = false;
		Rect clipChildrenRect = { { 0, 0 }, { 0, 0 } };
		int draggingButton = 0;
		bool wantTextInput = false;
		std::shared_ptr<ToolTipInfo> toolTip;
		Cursor::CursorType hoverCursor = Cursor::cursorArrow;
		
		int deferRemovalsDepth = 0;
		std::list<std::shared_ptr<Component>> children;
		std::list<Component *> childrenOrdered;
		bool hasDeferredRemovals = false;
		bool stealsMouse = false;
		Component *parent = nullptr;
		Component *childUnderMouse = nullptr;
		Component *childWithFocus = nullptr;
		Component *childWithMouseCapture = nullptr;
		std::list<std::shared_ptr<Component>>::iterator childrenIt;
		std::list<Component *>::iterator childrenOrderedIt;
		void ChildToFrontEnsureOrder(Component *component);
		void ChildToBackEnsureOrder(Component *component);
		bool underMouse = false;
		bool withFocus = false;
		SDL_Renderer *renderer = nullptr;

		template<class It, class VisitCancel>
		bool ChildrenUnordered(It begin, It end, VisitCancel visitCancel);

		template<class It, class VisitCancel>
		bool ChildrenOrdered(It begin, It end, VisitCancel visitCancel) const;

		template<class VisitCancel>
		bool Children(VisitCancel visitCancel);

		template<class VisitCancel>
		bool ChildrenForward(VisitCancel visitCancel) const;

		template<class VisitCancel>
		bool ChildrenReverse(VisitCancel visitCancel) const;

		void UpdateChildUnderMouse(const Event *higherEv);
		void SetChildWithFocus(Component *newChildWithFocus);
		void SetChildWithMouseCapture(Component *newChildWithMouseCapture);
		void UpdateAbsolutePosition();
		Component *FindChildUnderMouse(Point pos) const;

		Component(const Component &) = delete;
		Component &operator =(const Component &) = delete;

		class DeferRemovals
		{
			Component &component;

		public:
			DeferRemovals(Component &component);
			~DeferRemovals();
		};

	protected:
		virtual void Tick();
		virtual void Draw() const;
		virtual void DrawAfterChildren() const;
		virtual void Quit();
		virtual void FocusGain();
		virtual void FocusLose();
		virtual void MouseEnter(Point current);
		virtual void MouseLeave();
		virtual bool MouseMove(Point current, Point delta);
		virtual bool MouseDown(Point current, int button);
		virtual bool MouseUp(Point current, int button);
		virtual void MouseDragMove(Point current, int button);
		virtual void MouseDragBegin(Point current, int button);
		virtual void MouseDragEnd(Point current, int button);
		virtual void MouseClick(Point current, int button, int clicks);
		virtual bool MouseWheel(Point current, int distance, int direction);
		virtual bool KeyPress(int sym, int scan, bool repeat, bool shift, bool ctrl, bool alt);
		virtual bool KeyRelease(int sym, int scan, bool repeat, bool shift, bool ctrl, bool alt);
		virtual bool TextInput(const String &data);
		virtual void TextEditing(const String &data);
		virtual bool FileDrop(const String &path);
		virtual void RendererUp();
		virtual void RendererDown();

	public:
		Component() = default;
		virtual ~Component();

		bool HandleEvent(const Event &ev);
		bool HandleEvent(const Event &ev) const;
		bool InsertChild(std::shared_ptr<Component> component);
		bool RemoveChild(Component *component);
		bool ChildToFront(Component *component);
		bool ChildToBack(Component *component);

		template<class ComponentType, class ...Args>
		std::shared_ptr<ComponentType> EmplaceChild(Args &&...args)
		{
			auto ptr = std::make_shared<ComponentType>(std::forward<Args>(args)...);
			if (InsertChild(ptr))
			{
				return ptr;
			}
			return nullptr;
		}

		std::shared_ptr<Component> GetChild(Component *component) const;

		template<class ComponentType>
		typename std::enable_if<!std::is_same<ComponentType, Component>::value, std::shared_ptr<ComponentType>>::type GetChild(ComponentType *component) const
		{
			return std::static_pointer_cast<ComponentType>(GetChild(static_cast<Component *>(component)));
		}

		virtual void Position(Point newPosition);
		Point Position() const
		{
			return relativePosition;
		}
		Point AbsolutePosition() const
		{
			return absolutePosition;
		}

		virtual void Size(Point newSize);
		Point Size() const
		{
			return size;
		}

		void Enabled(bool newEnabled);
		bool Enabled() const
		{
			return enabled;
		}

		void Visible(bool newVisible);
		bool Visible() const
		{
			return visible;
		}

		bool WithFocus() const
		{
			return withFocus;
		}

		bool UnderMouse() const
		{
			return underMouse;
		}

		void MouseForwardRect(Rect newMouseForwardRect);
		Rect MouseForwardRect() const
		{
			return mouseForwardRect;
		}

		bool Dragging(int button) const
		{
			return draggingButton & (1 << button);
		}

		void ClipChildren(bool newClipChildren)
		{
			clipChildren = newClipChildren;
		}
		bool ClipChildren() const
		{
			return clipChildren;
		}

		void StealsMouse(bool newStealsMouse)
		{
			stealsMouse = newStealsMouse;
		}
		bool StealsMouse() const
		{
			return stealsMouse;
		}

		void ToFrontUnderMouse(bool newToFrontUnderMouse);
		bool ToFrontUnderMouse() const
		{
			return toFrontUnderMouse;
		}

		void ClipChildrenRect(Rect newClipChildrenRect)
		{
			clipChildrenRect = newClipChildrenRect;
		}
		Rect ClipChildrenRect() const
		{
			return clipChildrenRect;
		}

		Component *Parent() const
		{
			return parent;
		}

		void CaptureMouse(bool captureMouse);
		void Focus();

		void ToolTip(std::shared_ptr<ToolTipInfo> newToolTip)
		{
			toolTip = newToolTip;
		}
		std::shared_ptr<ToolTipInfo> ToolTip() const
		{
			return toolTip;
		}

		Component *ChildUnderMouse() const
		{
			return childUnderMouse;
		}

		Component *ChildUnderMouseDeep() const
		{
			auto *ret = childUnderMouse;
			if (ret)
			{
				auto *deep = ret->ChildUnderMouseDeep();
				if (deep)
				{
					ret = deep;
				}
			}
			return ret;
		}

		Component *ChildWithFocus() const
		{
			return childWithFocus;
		}

		Component *ChildWithFocusDeep() const
		{
			auto *ret = childWithFocus;
			if (ret)
			{
				auto *deep = ret->ChildWithFocusDeep();
				if (deep)
				{
					ret = deep;
				}
			}
			return ret;
		}

		SDL_Renderer *Renderer() const
		{
			return renderer;
		}

		void WantTextInput(bool newWantTextInput)
		{
			wantTextInput = newWantTextInput;
		}
		bool WantTextInput() const
		{
			return wantTextInput;
		}

		void HoverCursor(Cursor::CursorType newHoverCursor);
		Cursor::CursorType HoverCursor() const
		{
			return hoverCursor;
		}

		friend class DeferRemovals;
	};
}
