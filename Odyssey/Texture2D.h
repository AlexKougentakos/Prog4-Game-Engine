#pragma once
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

struct SDL_Texture;
namespace ody
{
	/**
	 * Simple RAII wrapper for an SDL_Texture
	 */
	class Texture2D final
	{
	public:
		explicit Texture2D(SDL_Texture* texture);
		~Texture2D();
		
		Texture2D(const Texture2D &) = delete;
		Texture2D(Texture2D &&) = delete;
		Texture2D & operator= (const Texture2D &) = delete;
		Texture2D & operator= (const Texture2D &&) = delete;
		
		SDL_Texture* GetSDLTexture() const;
		glm::ivec2 GetSize() const;
		void Tint(const glm::vec4& color);
	private:
		SDL_Texture* m_texture{};
		glm::vec4 m_CurrentTint{1.f, 1.f, 1.f, 1.f};
	};
}
