#include "DebugDrawer.h"

void ody::DebugDrawer::DrawSquare(float x, float y, float w, float h, bool persistent, SDL_Color color)
{
    if (persistent) m_PersistentShapes.emplace_back(DebugShape{ DebugShape::Type::Square, x, y, w, h, color });
    else m_Shapes.emplace_back(DebugShape{ DebugShape::Type::Square, x, y, w, h, color });
}

void ody::DebugDrawer::DrawCircle(float x, float y, float radius, bool persistent, SDL_Color color)
{
    if (persistent) m_PersistentShapes.emplace_back(DebugShape{ DebugShape::Type::Circle, x, y, radius, radius, color });
    else m_Shapes.emplace_back(DebugShape{ DebugShape::Type::Circle, x, y, radius, radius, color });
}

void ody::DebugDrawer::DrawLine(float x1, float y1, float x2, float y2, bool persistent, SDL_Color color)
{
    if (persistent) m_PersistentShapes.emplace_back(DebugShape{ DebugShape::Type::Line, x1, y1, x2, y2, color });
    else m_Shapes.emplace_back(DebugShape{ DebugShape::Type::Line, x1, y1, x2, y2, color });
}

void ody::DebugDrawer::Render(SDL_Renderer* renderer) const
{
    for (const auto& shape : m_Shapes)
    {
        switch (shape.type)
        {
        case DebugShape::Type::Square:
            RenderSquare(renderer, shape);
            break;
        case DebugShape::Type::Circle:
            RenderCircle(renderer, shape);
            break;
        case DebugShape::Type::Line:
            RenderLine(renderer, shape);
            break;
        }
    }

    for (const auto& shape : m_PersistentShapes)
    {
        switch (shape.type)
        {
        case DebugShape::Type::Square:
            RenderSquare(renderer, shape);
            break;
        case DebugShape::Type::Circle:
            RenderCircle(renderer, shape);
            break;
        case DebugShape::Type::Line:
            RenderLine(renderer, shape);
            break;
        }
    }
}

void ody::DebugDrawer::Clear()
{
    m_Shapes.clear();
}

void ody::DebugDrawer::ClearPersistent()
{
    m_PersistentShapes.clear();
}

void ody::DebugDrawer::RenderSquare(SDL_Renderer* renderer, const DebugShape& shape) const
{
    SDL_SetRenderDrawColor(renderer, shape.color.r, shape.color.g, shape.color.b, shape.color.a);
    const SDL_Rect rect = { static_cast<int>(shape.x), static_cast<int>(shape.y), static_cast<int>(shape.w), static_cast<int>(shape.h) };
    SDL_RenderDrawRect(renderer, &rect);
}

void ody::DebugDrawer::RenderCircle(SDL_Renderer* renderer, const DebugShape& shape) const
{
    // Simple circle drawing algorithm (you might want to improve this)
    SDL_SetRenderDrawColor(renderer, shape.color.r, shape.color.g, shape.color.b, shape.color.a);
    const int radius = static_cast<int>(shape.w / 2);
    const int centerX = static_cast<int>(shape.x + radius);
    const int centerY = static_cast<int>(shape.y + radius);

    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            const int dx = radius - w;
            const int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius))
            {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

void ody::DebugDrawer::RenderLine(SDL_Renderer* renderer, const DebugShape& shape) const
{
    SDL_SetRenderDrawColor(renderer, shape.color.r, shape.color.g, shape.color.b, shape.color.a);
    SDL_RenderDrawLine(renderer, static_cast<int>(shape.x), static_cast<int>(shape.y), static_cast<int>(shape.w), static_cast<int>(shape.h));
}
