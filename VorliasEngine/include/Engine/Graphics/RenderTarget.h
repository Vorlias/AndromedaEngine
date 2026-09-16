#pragma once
#include "Engine/Data/Color.h"
#include "Engine/Graphics/RenderCommand.h"
#include "imgui/imgui.h"

namespace andromeda::graphics {
	class RenderTarget {
	public:
		virtual ~RenderTarget() = default;

		virtual bool IsValid() const {
			return m_isValid;
		}

		virtual int GetWidth() const {
			return m_width;
		}

		virtual int GetHeight() const {
			return m_height;
		}

		virtual void Resize(int width, int height) {
			m_width = width;
			m_height = height;
		}

		virtual void Destroy() {
			m_isValid = false;
		}

		virtual void SetClearColor(Color color) = 0;
	protected:
		int m_width = 0;
		int m_height = 0;
		bool m_isValid = false;
	};

	class RenderSurface : RenderTarget {
	public:
	};

	enum class RenderTextureType {
		Texture,
		IMGUITexture,
	};

	class RenderTexture : public RenderTarget {
	public:
		static std::shared_ptr<RenderTexture> Create(RenderTextureType type = RenderTextureType::Texture);
		virtual ImTextureID GetImGuiTexture() const = 0;

		template<typename T, typename... Args>
			requires(std::is_base_of_v<andromeda::graphics::RenderCommand, T> && !std::is_same_v<andromeda::graphics::RenderCommand, T>)
		void Submit(Args&&... args) {
			std::unique_ptr<andromeda::graphics::RenderCommand> command = createCommand<T>(std::forward<Args>(args)...);
			SubmitCommand(std::move(command));
		}

		virtual void SubmitCommand(std::unique_ptr<andromeda::graphics::RenderCommand> command) {}
	};
} // namespace andromeda::graphics
