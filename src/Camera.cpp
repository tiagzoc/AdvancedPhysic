#include "Camera.h"

Camera::Camera()
{
	Proj = glm::perspective(
        glm::radians(60.0f),
        static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
        0.001f,
        1000.0f
    );
}

glm::mat4 Camera::GetView()
{
    glm::vec3 direction = transform.GetForward();

    return glm::lookAt(
        transform.GetPosition(),
        transform.GetPosition() + direction,
        {0, 1, 0}
    );
}
