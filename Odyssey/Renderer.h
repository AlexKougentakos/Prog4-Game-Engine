#pragma once
#include <memory>
#include <SDL.h>
#include "Singleton.h"

namespace ody
{
	class Texture2D;
	/**
	 * Simple RAII wrapper for the SDL renderer
	 */
	class Renderer final : public Singleton<Renderer>
	{
		SDL_Renderer* m_renderer{};
		SDL_Window* m_window{};
		SDL_Color m_clearColor{};	
	public:
		void Init(SDL_Window* window);
		void Render() const;
		void Destroy();

		void RenderTexture(const Texture2D& texture, float x, float y, float scale = 1.f) const;
		void RenderTexture(const Texture2D& texture, float x, float y, float width, float height, float scale = 1.f) const;
		void RenderTexture(const Texture2D& texture, float x, float y, float width, float height, float srcX, float srcY, float scale = 1.0f, SDL_RendererFlip flip = SDL_FLIP_NONE) const;
		void RenderTexture(const Texture2D& texture, float x, float y, float width, float height, float rotation,
		                   float scale,
		                   SDL_RendererFlip flip) const;

		SDL_Renderer* GetSDLRenderer() const;

		const SDL_Color& GetBackgroundColor() const { return m_clearColor; }
		void SetBackgroundColor(const SDL_Color& color) { m_clearColor = color; }
	};
}

