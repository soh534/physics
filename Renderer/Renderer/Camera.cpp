#include <Renderer/Renderer/Camera.h>

Camera::Camera()
{
	m_cameraPos = glm::vec3( 0.f, 0.f, -200.f );
	m_cameraDir = glm::vec3( 0.f, 0.f, 1.f );
	m_cameraUp = glm::vec3( 0.f, 1.f, 0.f );

	m_yaw = 90.f; // Start with facing z axis
	m_pitch = 0.f;
}

void Camera::panAndTilt( const float yaw, const float pitch )
{
	m_yaw += yaw;
	m_pitch += pitch;

	m_cameraDir.x = cos( glm::radians( m_pitch ) ) * cos( glm::radians( m_yaw ) );
	m_cameraDir.y = sin( glm::radians( m_pitch ) );
	m_cameraDir.z = cos( glm::radians( m_pitch ) ) * sin( glm::radians( m_yaw ) );
}

void Camera::dolly( const float x )
{
	m_cameraPos += m_cameraDir * x;
}

void Camera::pedestal(const float y)
{
	m_cameraPos += m_cameraUp * y;
}

void Camera::truck( const float z )
{
	m_cameraPos += glm::cross( m_cameraDir, m_cameraUp ) * z;
}
