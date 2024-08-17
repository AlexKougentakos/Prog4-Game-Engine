#include <SDL.h>
#include "Texture2D.h"

#include "Utils.h"

ody::Texture2D::Texture2D(SDL_Texture* texture)
{
	m_texture = texture;
}

ody::Texture2D::~Texture2D()
{
	SDL_DestroyTexture(m_texture);
}

glm::ivec2 ody::Texture2D::GetSize() const
{
	SDL_Rect dst;
	SDL_QueryTexture(GetSDLTexture(), nullptr, nullptr, &dst.w, &dst.h);
	return { dst.w,dst.h };
}

void ody::Texture2D::Tint(const glm::vec4& color)
{
	//I'm not sure how expensive these calls are so I'm making this
	//safe to call every frame
	if (Utils::AreEqual(color, m_CurrentTint)) return;
	m_CurrentTint = color;

	SDL_SetTextureColorMod(m_texture,
		static_cast<Uint8>(color.r * 255),
		static_cast<Uint8>(color.g * 255),
		static_cast<Uint8>(color.b * 255));
	SDL_SetTextureAlphaMod(m_texture, static_cast<Uint8>(color.a * 255));
}

SDL_Texture* ody::Texture2D::GetSDLTexture() const
{
	return m_texture;
}
