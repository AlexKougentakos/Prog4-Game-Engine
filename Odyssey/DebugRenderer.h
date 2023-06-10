#pragma once
#pragma warning(push, 0)
#include <SDL_render.h>
#include <SDL_rect.h>
#include <vector>
#include <Box2D/b2_draw.h>
#include <Box2D/b2_world.h>
#include "Constants.h"

#include "Singleton.h"
#include "Utils.h"

namespace ody
{

    class DebugRenderer : public ody::Singleton<DebugRenderer>, public b2Draw
    {
    public:
        void SetRenderer(SDL_Renderer* renderer)
        {
            m_Renderer = renderer;
            SetFlags(e_shapeBit | e_jointBit | e_pairBit | e_centerOfMassBit);
        }

        void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override
        {
            SDL_Color sdlColor = ConvertColor(color);
            SDL_SetRenderDrawColor(m_Renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);
            for (int32 i = 0; i < vertexCount; ++i)
            {
                SDL_RenderDrawLine(m_Renderer, Utils::MetersToPixels(vertices[i].x), Utils::MetersToPixels(vertices[i].y),
                    Utils::MetersToPixels(vertices[(i + 1) % vertexCount].x), Utils::MetersToPixels(vertices[(i + 1) % vertexCount].y));
            }
        }

        void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override
        {
            if (vertexCount <= 0) return;

            SDL_Color sdlColor = ConvertColor(color);
            SDL_SetRenderDrawColor(m_Renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);

            // Convert vertices to screen coordinates and create SDL_Rects
            std::vector<SDL_Rect> rects;
            rects.reserve(vertexCount - 2);
            for (int32 i = 1; i < vertexCount - 1; ++i)
            {
                SDL_Rect rect;
                rect.x = Utils::MetersToPixels(vertices[0].x);
                rect.y = Utils::MetersToPixels(vertices[0].y);
                rect.w = Utils::MetersToPixels(vertices[i].x - vertices[0].x);
                rect.h = Utils::MetersToPixels(vertices[i].y - vertices[0].y);
                rects.push_back(rect);
            }

            // Render filled polygon
            SDL_RenderFillRects(m_Renderer, rects.data(), rects.size());

            // Render outline
            SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, sdlColor.a);
            SDL_RenderDrawRects(m_Renderer, rects.data(), rects.size());
        }


        void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override
        {
            SDL_Color sdlColor = ConvertColor(color);
            SDL_SetRenderDrawColor(m_Renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);
            for (int w = 0; w < 360; w++)
            {
                SDL_RenderDrawPoint(m_Renderer, Utils::MetersToPixels(center.x + radius * cos(w)),
                    Utils::MetersToPixels(center.y + radius * sin(w)));
            }
        }

        void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override {
            // Convert Box2D coordinates to screen coordinates
            int screenX = Utils::MetersToPixels(center.x);
            int screenY = Utils::MetersToPixels(center.y);
            int screenRadius = Utils::MetersToPixels(radius);

            // Convert Box2D color to SDL color
            SDL_Color sdlColor;
            sdlColor.r = static_cast<Uint8>(color.r * 255);
            sdlColor.g = static_cast<Uint8>(color.g * 255);
            sdlColor.b = static_cast<Uint8>(color.b * 255);
            sdlColor.a = 255; // Set alpha to maximum

            // Set the color for drawing
            SDL_SetRenderDrawColor(m_Renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);

            // Draw the circle
            for (int w = 0; w < screenRadius * 2; w++) {
                for (int h = 0; h < screenRadius * 2; h++) {
                    int dx = screenRadius - w; // horizontal offset
                    int dy = screenRadius - h; // vertical offset
                    if ((dx * dx + dy * dy) <= (screenRadius * screenRadius)) {
                        SDL_RenderDrawPoint(m_Renderer, screenX + dx, screenY + dy);
                    }
                }
            }
        }

        void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override
        {
            SDL_Color sdlColor = ConvertColor(color);
            SDL_SetRenderDrawColor(m_Renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);
            SDL_RenderDrawLine(m_Renderer, Utils::MetersToPixels(p1.x), Utils::MetersToPixels(p1.y),
                Utils::Utils::MetersToPixels(p2.x), Utils::MetersToPixels(p2.y));
        }

        void DrawTransform(const b2Transform& xf) override
        {
            float scale = 0.4f;  // Length scale for the transform axes

            b2Vec2 p1 = xf.p, p2;
            const float k_axisScale = 0.4f;

            // Draw X-axis (red)
            p2 = p1 + k_axisScale * xf.q.GetXAxis();
            DrawSegment(p1, p2, b2Color(1, 0, 0));

            // Draw Y-axis (green)
            p2 = p1 + k_axisScale * xf.q.GetYAxis();
            DrawSegment(p1, p2, b2Color(0, 1, 0));
        }

        void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override {
            // Convert Box2D's color (0.0 to 1.0) to SDL's color (0 to 255)
            SDL_SetRenderDrawColor(m_Renderer, color.r * 255, color.g * 255, color.b * 255, 255);

            // Convert Box2D's coordinates to screen coordinates
            int x = Utils::MetersToPixels(p.x);
            int y = constants::g_ScreenHeight - Utils::MetersToPixels(p.y);

            // Convert size from meters to pixels
            int pixelSize = Utils::MetersToPixels(size);

            // Draw the point as a small rectangle
            SDL_Rect rect;
            rect.x = x - pixelSize / 2;
            rect.y = y - pixelSize / 2;
            rect.w = pixelSize;
            rect.h = pixelSize;
            SDL_RenderFillRect(m_Renderer, &rect);
        }

    void SetWorld(b2World* world)
    {
        m_pWorld = world;
    }

    void Render()
    {
        m_pWorld->DebugDraw();
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
