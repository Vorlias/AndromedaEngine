#pragma once
#include "lua.h"
#include "lualib.h"
#include "Engine/Luau/Lib.h"
#include "Engine/Luau/LuauScript.h"
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
		andromeda::LuauStateContext getContext;

		std::function<int(lua_State*, T*)> set{};
		andromeda::LuauStateContext setContext;
	};

	template<typename C, typename R, typename... Args>
	using Method = R (C::*)(Args...);

	template<typename R, typename... Args>
	using Func = R (*)(Args...);

	template<typename T>
	struct LuauMethod {
		const char* name;
		std::function<int(lua_State*, T*)> invoke{};
		andromeda::LuauStateContext context;
	};

	template<typename T>
	class LuauUserdataBuilder;

	template<typename T>
	class LuauUserdataType {
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
		using CustomHandler = int (*)(lua_State* L, T* obj);

		template<typename R>
		using Getter = R (T::*)() const;
		template<typename V>
		using Setter = void (T::*)(V);
		template<typename V>
		using Field = V T::*;

		LuauUserdataType() : m_name(nullptr), m_tag(0) {}
		LuauUserdataType(const char* name, int tag = 0) : m_name(name), m_tag(tag) {}

		LuauUserdataType<T>& SetName(const char* name) {
			m_name = name;
			return *this;
		}

		LuauUserdataType<T>& SetTag(int tag) {
			ANDROMEDA_ASSERT(tag >= 0 && tag < LUA_UTAG_LIMIT);
			m_tag = tag;
			return *this;
		}

		template<typename V>
		LuauUserdataType<T>& AddField(const char* name, Field<V> member, andromeda::LuauStateContext context = andromeda::LuauStateContext::ALL) {
			LuauProperty<T>& property = GetOrCreateProperty(name);
			ANDROMEDA_ASSERT(property.get == nullptr);
			ANDROMEDA_ASSERT(property.set == nullptr);
			int contextInt = static_cast<int>(context);

			property.get = [member, contextInt, name](lua_State* L, T* obj) -> int {
				LuauStack stack(L);
				int callContextInt = static_cast<int>(andromeda::LuauState::GetContextFromState(L));
				if ((callContextInt & contextInt) == 0) {
					luaL_errorL(L, "Call to get on field '%s' is not valid in this context", name);
					return 0;
				}

				stack.PushValue(obj->*member);
				return 1;
			};

			property.set = [member, &property, contextInt, name](lua_State* L, T* obj) -> int {
				LuauStack stack(L);
				int callContextInt = static_cast<int>(andromeda::LuauState::GetContextFromState(L));
				if ((callContextInt & contextInt) == 0) {
					luaL_errorL(L, "Call to set on field '%s' is not valid in this context", name);
					return 0;
				}

				if (!stack.IsType<V>(-1)) {
					luaL_errorL(L, "Attempt to assign %s to %s property '%s'", luaL_typename(L, -1), stack.GetTypeName<V>(), property.name);
				}

				auto value = stack.GetValue<V>(-1);
				obj->*member = value;
				return 0;
			};
			return *this;
		}

		template<typename R>
		LuauUserdataType<T>& AddGetter(const char* name, Getter<R> getter, andromeda::LuauStateContext context = andromeda::LuauStateContext::ALL) {
			LuauProperty<T>& property = GetOrCreateProperty(name);
			ANDROMEDA_ASSERT(property.get == nullptr);
			int contextInt = static_cast<int>(context);

			property.get = [getter, contextInt, name](lua_State* L, T* obj) -> int {
				LuauStack stack(L);

				int callContextInt = static_cast<int>(andromeda::LuauState::GetContextFromState(L));
				if ((callContextInt & contextInt) == 0) {
					luaL_errorL(L, "Call to property get '%s' is not valid in this context", name);
					return 0;
				}

				R value = (obj->*getter)();
				stack.PushValue<R>(value);
				return 1;
			};
			return *this;
		}

		template<typename V>
		LuauUserdataType<T>& AddSetter(const char* name, Setter<V> setter, andromeda::LuauStateContext context = andromeda::LuauStateContext::ALL) {
			LuauProperty<T>& property = GetOrCreateProperty(name);
			ANDROMEDA_ASSERT(property.set == nullptr);
			int contextInt = static_cast<int>(context);

			property.set = [setter, &property, contextInt](lua_State* L, T* obj) -> int {
				LuauStack stack(L);
				auto callContextInt = static_cast<int>(andromeda::LuauState::GetContextFromState(L));

				if ((callContextInt & contextInt) == 0) {
					luaL_errorL(L, "Call to property set '%s' is not valid in this context", property.name);
					return 0;
				}

				if (!stack.IsType<V>(-1)) {
					luaL_errorL(L, "Attempt to assign %s to %s property '%s'", luaL_typename(L, -1), stack.GetTypeName<V>(), property.name);
				}

				V value = stack.GetValue<V>(-1);
				(obj->*setter)(value);
				return 0;
			};

			return *this;
		}

		LuauUserdataType<T>& AddGetter(const char* name, CustomHandler getter) {
			LuauProperty<T>& property = GetOrCreateProperty(name);
			property.get = getter;
			return *this;
		}

		LuauUserdataType<T>& AddSetter(const char* name, CustomHandler setter) {
			LuauProperty<T>& property = GetOrCreateProperty(name);
			property.set = setter;
			return *this;
		}

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
			return m_name != nullptr && (m_properties.size() > 0 || m_methods.size() > 0);
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
} // namespace andromeda_luau
