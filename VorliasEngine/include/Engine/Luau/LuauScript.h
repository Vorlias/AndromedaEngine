#pragma once
#include <string>
#include "lua.h"
#include "LuauState.h"
#include "Engine/Memory.h"
#include "Engine/Asset.h"
#include "Engine/Objects/Object.h"
#include <memory.h>


#define BYTECODE_MAX_LEN 1024 * 512
namespace andromeda {
	typedef char* bytecode_t;

	struct LuauBytecodeVersion {
		int min;
		int max;
		int target;
	};

	struct LuauBytecode {
		LuauBytecodeVersion version;
		size_t size;
		bytecode_t data;
	};

	class LuauScript : public Asset {
		bool CompileSource(const char* source, int source_len, const char* file_name, int file_name_len, int optimization_level);

	public:
		bool Compile(std::string_view source, const std::string& fileName = "chunk") {
			SetFilePath(fileName);
			return CompileSource(source.data(), source.length(), fileName.data(), fileName.length(), 2);
		}

		void LoadBytecode(const LuauBytecode& bytecode, const std::string& fileName) {
			if (bytecode.size > BYTECODE_MAX_LEN) {
				return;
			}

			memcpy(m_bytecode, (char*)bytecode.data, bytecode.size);
			SetFilePath(fileName);
		}

		constexpr bool HasErrored() const {
			return m_errlen > 0;
		}
		constexpr bool IsCompiled() const {
			return m_bytecodeSize > 0;
		}

		constexpr std::string GetError() const {
			return std::string(m_err, m_errlen);
		}

		[[nodiscard]] const LuauBytecode GetBytecode() const;

		LuauScript();
		~LuauScript();

		static const LuauBytecodeVersion GetBytecodeVersion();

		// Creates a script and the associated thread for it
		[[nodiscard]] static constexpr Ref<LuauScript> CreateScript(std::string_view source, const std::string& fileName) {
			auto ref = Ref<LuauScript>::Create();
			ref->Compile(source, fileName);
			return ref;
		};

		[[nodiscard]] static constexpr Ref<LuauScript> CreateScript(const LuauBytecode& bytecode, const std::string& fileName) {
			auto ref = Ref<LuauScript>::Create();
			ref->LoadBytecode(bytecode, fileName);
			return ref;
		}

	private:
		// lua_State* m_thread = nullptr;

		std::string m_fileName;

		char m_err[128];
		size_t m_errlen = 0;

		char m_bytecode[BYTECODE_MAX_LEN];
		size_t m_bytecodeSize = 0;
	};

	enum class LuauThreadStatus {
		// Script is still runnning
		Running = 0,
		// Script is suspended
		Suspended,
		// Script resumed another coroutine
		Normal,
		// script has finished execution
		Finished,
		// script finished execution with error
		Errored,
	};

	class LuauScriptThread {
	public:
		struct Cleanup {
			void operator()(LuauScriptThread* p) {
				delete p;
			}
		};

	private:
		bool Create();

	public:
		LuauScriptThread(Ref<LuauScript> script) : m_script(script) {
			Create();
		}
		[[nodiscard]] constexpr Ref<LuauScript> GetScript() {
			return m_script;
		}

		// Runs the thread
		bool Run();

		// Closes this thread and all attached sub-threads
		void Close();

		[[nodiscard]] LuauThreadStatus GetThreadStatus() const;
		[[nodiscard]] constexpr bool IsRunning() const {
			auto status = GetThreadStatus();
			return status != LuauThreadStatus::Finished && status != LuauThreadStatus::Errored;
		}

		~LuauScriptThread();

		constexpr lua_State* GetLuauState() const {
			ANDROMEDA_ASSERT(this != nullptr);
			return m_thread;
		}

		operator lua_State*() const {
			ANDROMEDA_ASSERT(this != nullptr);
			ANDROMEDA_ASSERT(m_thread != nullptr);
			return m_thread;
		}
	private:
		Ref<LuauScript> m_script;
		lua_State* m_thread = nullptr;
		bool m_running;
		friend class LuauScriptComponent;
	};

	struct LuauValue {
		enum LuauValueType {
			String,
			Int32,
			Float,
			Double,
			Bool,
		};

		union {
			const char* s;
			int i;
			float f;
			double d;
			void* p;
		} data;

		int size{};
		LuauValueType type;

		static const LuauValue& string(std::string_view str) {
			LuauValue value;
			value.type = String;
			value.data.s = str.data();
			value.size = str.size();

			return value;
		}

		static const LuauValue number(float f) {
			LuauValue value;
			value.type = Float;
			value.data.f = f;
			return value;
		}

		static const LuauValue boolean(bool b) {
			LuauValue value;
			value.type = Bool;
			value.data.i = !!b;
			return value;
		}

	private:
		LuauValue() {}
	};

	class LuauScriptComponent {
	public:
		enum State {
			// Hasn't yet awoken
			STATE_ASLEEP,
			
			// Has awoken
			STATE_AWAKE,

			// Has started
			STATE_STARTED,

			// Has closed
			STATE_CLOSED,
		};

		TAG_COMPONENT(UpdateLifecycle)

		TAG_COMPONENT(EnableLifecycle)
		TAG_COMPONENT(DisableLifecycle)

		LuauScriptComponent() : m_script(nullptr), m_entity() {}
		LuauScriptComponent(Ref<LuauScript> script) : m_script(script), m_entity() {}

		void SetScript(Ref<LuauScript> script);
		void SetEnabled(bool enabled);

		void Awake();
		void Start();
		void Update(float dt) const;
		void Shutdown();

		void Reset();

		void SetProperty(std::string_view property, LuauValue value);

		constexpr bool IsEnabled() const {
			return m_enabled;
		}

		constexpr bool IsAwake() const {
			return m_awake;
		}

		constexpr bool HasStarted() const {
			return m_start;
		}

		constexpr bool HasError() const {
			return m_err;
		}

		constexpr State GetState() const {
			return m_state;
		}

	private:
		Ref<LuauScript> m_script = nullptr;
		std::unique_ptr<LuauScriptThread, LuauScriptThread::Cleanup> m_thread{};

		Entity m_entity;
		State m_state = STATE_ASLEEP;

		bool m_enabled = true;
		bool m_awake;
		bool m_start;

		bool m_err;
		friend class Scene;
	};

	inline std::string to_string(LuauThreadStatus status) {
		switch (status) {
			using enum LuauThreadStatus;
			case Running:
				return _STR(Running);
			case Suspended:
				return _STR(Suspended);
			case Normal:
				return _STR(Normal);
			case Finished:
				return _STR(Finished);
			case Errored:
				return _STR(Errored);
			default:
				return "";
		}
	}
} // namespace andromeda