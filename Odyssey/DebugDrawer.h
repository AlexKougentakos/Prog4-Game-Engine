#pragma once
#include <vector>
#include <SDL.h>
#include "Singleton.h"

namespace ody
{
    struct DebugShape
    {
        enum class Type { Square, Circle, Line };
        Type type;
        float x, y, w, h;
        SDL_Color color;
    };

    class DebugDrawer final : public ody::Singleton<DebugDrawer>
    {
    public:
        void DrawSquare(float x, float y, float w, float h, SDL_Color color = { 255, 0, 0, 255 });
        void DrawCircle(float x, float y, float radius, SDL_Color color = { 0, 255, 0, 255 });
        void DrawLine(float x1, float y1, float x2, float y2, SDL_Color color = { 0, 0, 255, 255 });
        void Render(SDL_Renderer* renderer);
        void Clear();

    private:
        std::vector<DebugShape> m_Shapes;
        DebugDrawer() {}
        friend class Singleton<DebugDrawer>;

        void RenderSquare(SDL_Renderer* renderer, const DebugShape& shape);
        void RenderCircle(SDL_Renderer* renderer, const DebugShape& shape);
        void RenderLine(SDL_Renderer* renderer, const DebugShape& shape);
    };
}
