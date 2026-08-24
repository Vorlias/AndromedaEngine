#pragma once
#include "lua.h"
#include "lualib.h"
#include "Engine/Luau/Lib.h"
#include "Stack.h"
#include <limits>
#include <functional>
#include <unordered_map>

namespace andromeda_luau {
	struct StrHash {
		using is_transparent = void;
		size_t operator()(std::string_view sv) const noexcept {
			return std::hash<std::string_view>{}(sv);
		}
	};

	template<typename T>
	using string_map = std::unordered_map<std::string, T, StrHash, std::equal_to<>>;

	template<typename T>
	struct LuauProperty {
		const char* name;
		std::function<int(lua_State*, T*)> get{};
		std::function<int(lua_State*, T*)> set{};
	};

	template<typename T>
	struct LuauMethod {
		const char* name;
		std::function<int(lua_State*, T*)> invoke;
	};

	template<typename T>
	class LuauUserdataBuilder;

	template<typename T>
	class LuauUserdataType {
	public:
		LuauMethod<T>* FindMethod(const std::string& name) {
			if (m_methods.contains(name)) {
				return &m_methods.at(name);
			}

			return nullptr;
		}

		LuauProperty<T>* FindProperty(const std::string& name) {
			if (m_properties.contains(name)) {
				return &m_properties.at(name);
			}

			return nullptr;
		}

		operator bool() const {
			return m_name != nullptr;
		}

		constexpr int GetTag() const {
			return m_tag;
		}
		constexpr const char* GetName() const {
			return m_name;
		}
		const string_map<LuauMethod<T>>& GetMethods() const {
			return m_methods;
		}
		const string_map<LuauProperty<T>>& GetProperties() const {
			return m_properties;
		}

	private:
		friend class LuauUserdataBuilder<T>;
		const char* m_name{};
		int m_tag{0};
		string_map<LuauMethod<T>> m_methods{};
		string_map<LuauProperty<T>> m_properties{};
	};

	template<typename T>
	class LuauUserdataBuilder {
	private:
		LuauProperty<T>& GetOrCreateProperty(const char* name) {
			if (m_properties.contains(name)) {
				return m_properties.at(name);
			} else {
				LuauProperty<T> newProp;
				newProp.name = name;
				newProp.get = nullptr;
				newProp.set = nullptr;
				m_properties.insert({name, newProp});
				return m_properties.at(name);
			}
		}

	public:
		template<typename R>
		using Getter = R (T::*)() const;
		template<typename V>
		using Setter = void (T::*)(V);

		using CustomHandler = int (*)(lua_State* L, T* obj);

		template<typename V>
		using Member = V T::*;

		LuauUserdataBuilder(const char* name) : m_name(name) {}
		LuauUserdataBuilder(const char* name, int tag) : m_name(name), m_tag(tag) {}

		template<typename R>
		LuauUserdataBuilder& AddGetter(const char* name, Getter<R> getter) {
			LuauProperty<T>& property = GetOrCreateProperty(name);
			property.get = [getter](lua_State* L, T* obj) -> int {
				LuauStack stack(L);
				stack.PushValue<R>((obj->*getter)());
				return 1;
			};
			return *this;
		}

		LuauUserdataBuilder& AddGetter(const char* name, CustomHandler getter) {
			LuauProperty<T>& property = GetOrCreateProperty(name);
			property.get = getter;
			return *this;
		}

		LuauUserdataBuilder& AddSetter(const char* name, CustomHandler setter) {
			LuauProperty<T>& property = GetOrCreateProperty(name);
			property.set = setter;
			return *this;
		}

		template<typename V>
		LuauUserdataBuilder& AddSetter(const char* name, Setter<V> setter) {
			LuauProperty<T>& property = GetOrCreateProperty(name);
			property.set = [setter](lua_State* L, T* obj) -> int {
				LuauStack stack(L);
				V value = stack.GetValue<V>(-1);
				(obj->*setter)(value);
				return 0;
			};

			return *this;
		}

		template<typename V>
		LuauUserdataBuilder& AddField(const char* name, Member<V> member) {
			LuauProperty<T> property;
			property.name = name;

			property.get = [member](lua_State* L, T* obj) -> int {
				LuauStack stack(L);
				stack.PushValue(obj->*member);
				return 1;
			};

			property.set = [member](lua_State* L, T* obj) -> int {
				LuauStack stack(L);
				ANDROMEDA_ASSERT(stack.IsType<V>(-1));
				auto value = stack.GetValue<V>(-1);
				obj->*member = value;
				return 0;
			};

			m_properties.insert({name, property});
			return *this;
		}

		LuauUserdataType<T> Build() {
			LuauUserdataType<T> ud;
			ud.m_name = m_name;
			ud.m_tag = m_tag;
			ud.m_methods = m_methods;
			ud.m_properties = m_properties;
			return std::move(ud);
		};

		const char* m_name;
		int m_tag{0};
		string_map<LuauMethod<T>> m_methods{};
		string_map<LuauProperty<T>> m_properties{};
	};
} // namespace andromeda_luau
