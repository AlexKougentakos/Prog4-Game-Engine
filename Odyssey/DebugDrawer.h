#pragma once
#include <vector>
#include <SDL.h>
#include "Singleton.h"

namespace ody
{
    struct DebugShape
    {
        enum class Type { Square, Circle, Line };
        Type type{};
        float x, y, w, h{};
        SDL_Color color{};
    };

    class DebugDrawer final : public ody::Singleton<DebugDrawer>
    {
    public:
        void DrawSquare(float x, float y, float w, float h, bool persistent = false, SDL_Color color = { 255, 0, 0, 255 });
        void DrawCircle(float x, float y, float radius, bool persistent = false, SDL_Color color = { 0, 255, 0, 255 });
        void DrawLine(float x1, float y1, float x2, float y2, bool persistent = false, SDL_Color color = { 0, 0, 255, 255 });
        void Render(SDL_Renderer* renderer) const;
        void Clear();
        void ClearPersistent();

    private:
        std::vector<DebugShape>m_Shapes;
        //The reason for separating them is just so I don't have to loop over the entire array to check every time they get cleared (every frame)
        std::vector<DebugShape>m_PersistentShapes;
        DebugDrawer() {}
        friend class Singleton<DebugDrawer>;

        void RenderSquare(SDL_Renderer* renderer, const DebugShape& shape) const;
        void RenderCircle(SDL_Renderer* renderer, const DebugShape& shape) const;
        void RenderLine(SDL_Renderer* renderer, const DebugShape& shape) const;
    };
}
