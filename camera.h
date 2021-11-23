#ifndef _CAMERA_H
#define _CAMERA_H

#include "common.h"
#include "config.h"

/*
   CAMERA类，其最基本的组成部分为XYZ三个轴（FRONT,UP,RIGHT）以及其位置
*/
class Camera
{
private:
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	float yaw;
	float pitch;
	float speed;
	float sensity;
	float fov;
	float ratio;
	float near;
	float far;

public:
	enum MoveType {
		eFront,
		eBack,
		eLeft,
		eRight
	};

	glm::vec3 pos;
	glm::vec3 front;

	Camera(glm::vec3 pos, glm::vec3 target, float fov = 45.f,
		float near = 0.1f, float far = 250.0f, float ratio = 4.f/3.f,
		float speed = 1.0f, float sensity = 0.1f) :
		pos(pos), yaw(-90.f), pitch(0.f), speed(speed), sensity(sensity), worldUp(glm::vec3(0, 1, 0)),
		front(glm::normalize(target - pos)), right(glm::normalize(glm::cross(front, worldUp))), ratio(ratio), fov(fov), far(far), near(near) {}
	virtual glm::mat4 view_matrix() { return lookAt(pos, pos + front, worldUp); }
	virtual glm::mat4 projection_maxtrix() { return glm::perspective(glm::radians(fov), (float)ratio, near, far); }


	virtual void rotateViewDir(float xoff, float yoff);
	virtual void changeDirection(MoveType mtype);
	virtual void updateFov(float fov);
};

#endif // !#define _CAMERA_H
