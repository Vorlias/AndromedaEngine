#pragma once

#define EVENT_TYPE(T) \
	virtual EventType GetEventType() const { \
		return T; \
	} \
	virtual const char* GetName() const { \
		return #T; \
	} \
	static EventType GetStaticType() { \
		return T; \
	}


#define EVENT_CATEGORY(T) \
	virtual int GetCategoryFlags() const { \
		return T; \
	}



namespace andromeda {
	enum class EventType {
		WindowResized,
		WindowMoved,

		WindowVisibilityChanged,
		WindowFullscreenChanged,

		WindowClosed,
		WindowFocused,
		WindowFocusLost,

		WindowMinimized,
		WindowMaximized,
		WindowRestored,

		// KeyPressed,
		// KeyTyped,
		// KeyReleased,

		// MouseButtonPressed,
		// MouseButtonReleased,
		// MouseScrolled,
		// MouseMovement,
	};

	enum EventCategory {
		None,
		EventCategoryApplication = 1 << 0,
		EventCategoryInput = 1 << 1,
		EventCategoryKeyboard = 1 << 2,
		EventCategoryMouse = 1 << 3,
		EventCategoryMouseButton = 1 << 4,
	};

	class Event {
	public:
		virtual ~Event() = default;
		bool handled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const {
			return GetName();
		}

		bool IsInCategory(EventCategory category) {
			return GetCategoryFlags() & category;
		}
	};

	template<typename C, typename R, typename... Args>
	using Method = R (C::*)(Args...);

	template<typename R, typename... Args>
	using Func = R (*)(Args...);

	class EventDispatcher {
	public:
		EventDispatcher(Event& event) : event(event) {}

		// Dispatch the event to a method if it matches
		template<typename T, typename C>
			requires std::is_base_of_v<Event, T> 
		bool Dispatch(C* obj, Method<C, bool, T&> dispatcher) {
			if (event.GetEventType() == T::GetStaticType()) {
				event.handled |= (obj->*dispatcher)(static_cast<T&>(event));
				return true;
			}

			return false;
		}
	private:
		Event& event;
	};
} // namespace andromeda


namespace andromeda {
	// events

	class WindowFocusedEvent : public Event {
	public:
		WindowFocusedEvent() {}

	public:
		EVENT_TYPE(EventType::WindowFocused)
		EVENT_CATEGORY(EventCategoryApplication)
	};

	class WindowFocusLostEvent : public Event {
	public:
		WindowFocusLostEvent() {}

	public:
		EVENT_TYPE(EventType::WindowFocusLost)
		EVENT_CATEGORY(EventCategoryApplication)
	};

	class WindowResizedEvent : public Event {
	public:
		WindowResizedEvent(int32_t width, int32_t height) : width(width), height(height) {}

	public:
		EVENT_TYPE(EventType::WindowResized)
		EVENT_CATEGORY(EventCategoryApplication)

		int32_t width, height;
	};

	class WindowMovedEvent : public Event {
	public:
		WindowMovedEvent(int32_t x, int32_t y) : x(x), y(y) {}

	public:
		EVENT_TYPE(EventType::WindowMoved)
		EVENT_CATEGORY(EventCategoryApplication)

		int32_t x, y;
	};

	class WindowVisibilityChangedEvent : public Event {
	public:
		WindowVisibilityChangedEvent(bool visible) : visible(visible) {}

	public:
		EVENT_TYPE(EventType::WindowVisibilityChanged)
		EVENT_CATEGORY(EventCategoryApplication)

		bool visible;
	};

	class WindowFullscreenChangedEvent : public Event {
	public:
		WindowFullscreenChangedEvent(bool fullscreen) : fullscreen(fullscreen) {}

	public:
		EVENT_TYPE(EventType::WindowFullscreenChanged)
		EVENT_CATEGORY(EventCategoryApplication)

		bool fullscreen;
	};

	class WindowMaximizedEvent : public Event {
	public:
		WindowMaximizedEvent() {}

	public:
		EVENT_TYPE(EventType::WindowMaximized)
		EVENT_CATEGORY(EventCategoryApplication)
	};

	class WindowMinimizedEvent : public Event {
	public:
		WindowMinimizedEvent() {}

	public:
		EVENT_TYPE(EventType::WindowMinimized)
		EVENT_CATEGORY(EventCategoryApplication)
	};

	class WindowRestoredEvent : public Event {
	public:
		WindowRestoredEvent() {}

	public:
		EVENT_TYPE(EventType::WindowRestored)
		EVENT_CATEGORY(EventCategoryApplication)
	};
} // namespace andromeda