#pragma once
#include "Engine/Data/Color.h"

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

		virtual void Bind() {}
		virtual void Unbind() {}

		virtual void SetClearColor(Color color) = 0;
	protected:
		int m_width = 0;
		int m_height = 0;
		bool m_isValid = false;
	};
} // namespace andromeda::graphics
