#include "camera.h"

void Camera::changeDirection(Camera::MoveType mtype)
{
	if (mtype == MoveType::eFront)
		pos += this->front * speed;
	if (mtype == MoveType::eBack)
		pos -= this->front * speed;
	if (mtype == MoveType::eLeft)
		pos -= this->right * speed;
	if (mtype == MoveType::eRight)
		pos += this->right * speed;

#ifdef DEBUG_CAMEMRA
	std::cout << pos.x << ", "
		<< pos.y << ", "
		<< pos.z << std::endl;
#endif // DEBUG_CAMEMRA

}


void Camera::rotateViewDir(float xoff, float yoff)
{
	xoff *= sensity;
	yoff *= sensity;

	yaw += xoff;
	pitch += yoff;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	front = glm::normalize(front);
	right = normalize(cross(front, worldUp));

#ifdef DEBUG_CAMERA
	
	std::cout << "rotate: " << xoff << "," << yoff << std::endl;
	std::cout << "yaw: " << yaw << std::endl;
	std::cout << "    "
		<< front.x << ", "
		<< front.y << ", "
		<< front.z
		<< "    " << std::endl;
#endif // DEBUG_CAMERA

	
}

void Camera::updateFov(float fov)
{ 
	this->fov = fov; 

#ifdef DEBUG_CAMERA
	
	std::cout << "fov:" << fov << std::endl; 

#endif
}