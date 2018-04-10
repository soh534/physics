#pragma once

#include <glm/glm.hpp>

class Camera
{
public:

	Camera();

	void panAndTilt( const float yaw, const float pitch );
	void dolly( const float x );
	void pedestal(const float y);
	void truck(const float z);

	const glm::vec3& getPos() const { return m_cameraPos; }
	const glm::vec3& getDir() const { return m_cameraDir; }
	const glm::vec3& getUp() const { return m_cameraUp; }

	const float getYaw() const { return m_yaw; }
	const float getPitch() const { return m_pitch; }

private:

	glm::vec3 m_cameraPos;
	glm::vec3 m_cameraDir;
	glm::vec3 m_cameraUp;

	float m_yaw, m_pitch;
};
