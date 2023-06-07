#pragma once
#pragma warning(push, 0)
#include <SDL_render.h>
#include <Box2D/Common/b2Draw.h>
#include <Box2D/Dynamics/b2World.h>

#include "Singleton.h"

namespace ody
{

class DebugRenderer : public ody::Singleton<DebugRenderer>, public b2Draw
{
public:
    void SetRenderer(SDL_Renderer* renderer)
    {
        m_Renderer = renderer;
        SetFlags(e_shapeBit | e_jointBit | e_pairBit | e_centerOfMassBit | e_aabbBit);
    }

    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override
    {
        SDL_Color sdlColor = ConvertColor(color);
        SDL_SetRenderDrawColor(m_Renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);
        for (int32 i = 0; i < vertexCount; ++i)
        {
            SDL_RenderDrawLine(m_Renderer, vertices[i].x, vertices[i].y, vertices[(i + 1) % vertexCount].x, vertices[(i + 1) % vertexCount].y);
        }
    }

    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override
    {
        // Solid polygons can be complex to implement with SDL, consider using SDL2_gfx or similar.
        DrawPolygon(vertices, vertexCount, color);
    }

    void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override
    {
        SDL_Color sdlColor = ConvertColor(color);
        SDL_SetRenderDrawColor(m_Renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);
        for (int w = 0; w < 360; w++)
        {
            SDL_RenderDrawPoint(m_Renderer, center.x + radius * cos(w), center.y + radius * sin(w));
        }
    }

    void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override
    {
        // Filled circles can be complex to implement with SDL, consider using SDL2_gfx or similar.
        DrawCircle(center, radius, color);
    }

    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override
    {
        SDL_Color sdlColor = ConvertColor(color);
        SDL_SetRenderDrawColor(m_Renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);
        SDL_RenderDrawLine(m_Renderer, p1.x, p1.y, p2.x, p2.y);
    }

    void DrawTransform(const b2Transform& xf) override
    {
        float32 scale = 0.4f;  // Length scale for the transform axes

        b2Vec2 p1 = xf.p, p2;
        const float32 k_axisScale = 0.4f;

        // Draw X-axis (red)
        p2 = p1 + k_axisScale * xf.q.GetXAxis();
        DrawSegment(p1, p2, b2Color(1, 0, 0));

        // Draw Y-axis (green)
        p2 = p1 + k_axisScale * xf.q.GetYAxis();
        DrawSegment(p1, p2, b2Color(0, 1, 0));
    }

    void SetWorld(b2World* world)
    {
        m_pWorld = world;
    }

    void Render()
    {
        m_pWorld->DrawDebugData();
    }

private:
    SDL_Renderer* m_Renderer{ nullptr };
    b2World* m_pWorld{ nullptr };

    SDL_Color ConvertColor(const b2Color& color)
    {
        SDL_Color sdlColor;
        sdlColor.r = static_cast<Uint8>(color.r * 255);
        sdlColor.g = static_cast<Uint8>(color.g * 255);
        sdlColor.b = static_cast<Uint8>(color.b * 255);
        sdlColor.a = SDL_ALPHA_OPAQUE;
        return sdlColor;
    }


};
#pragma warning(pop)
}
