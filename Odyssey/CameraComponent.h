#pragma once
#include <glm/vec2.hpp>

#include "Component.h"


namespace ody
{
	class CameraComponent : public Component
	{
    public:
        CameraComponent() : position(0.0f), zoom(1.0f) {}

        void Update() override
		{
            // Implement camera logic (e.g., movement, zoom)
        }

        void SetPosition(const glm::vec2& newPosition) { position = newPosition; }

        glm::vec2 GetPosition() const { return position; }

        void SetZoom(const float newZoom) { zoom = newZoom; }

        float GetZoom() const { return zoom; }
    private:
        glm::vec2 position;
        float zoom;
	};
}