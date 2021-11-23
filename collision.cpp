#include "collision.h"
#include "util.h"
#include <cfloat>
#include <climits>

// ------------------------------ AABB CLASS -------------------------- //
AABB::AABB()
{
	this->min = glm::vec3(FLT_MAX);
	this->max = glm::vec3(FLT_MIN);
	memset(this->boxPoints, 0, sizeof(glm::vec3) * 8);
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         

AABB::AABB(glm::vec3 min, glm::vec3 max)
{ 
	set(min, max);
}

void AABB::set(glm::vec3 min, glm::vec3 max)
{
	this->min = min;
	this->max = max;
	updateBoxPoint();
}

void AABB::update(glm::mat4 mtx)
{
	glm::vec4 aabbmin(min, 1.f);
	glm::vec4 aabbmax(max, 1.f);
	min = vec42vec3(mtx * aabbmin);
	max = vec42vec3(mtx * aabbmax);

}

AABB::AABB(FbxVector4* controlPoints, int n) : AABB()
{
	
	for (size_t i = 0; i < n; i++)
	{
		FbxVector4 point = controlPoints[i];
		if (point[0] > max.x)
			max.x = point[0];
		if (point[1] > max.y)
			max.y = point[1];
		if (point[2] > max.z)
			max.z = point[2];

		if (point[0] < min.x)
			min.x = point[0];
		if (point[1] < min.y)
			min.y = point[1];
		if (point[2] < min.z)
			min.z = point[2];
	}
	updateBoxPoint();
}
void AABB::updateBoxPoint()
{
	boxPoints[0] = min;
	boxPoints[1] = glm::vec3(min.x, min.y, max.z);
	boxPoints[2] = glm::vec3(max.x, min.y, min.z);
	boxPoints[3] = glm::vec3(min.x, max.y, min.z);
	boxPoints[4] = glm::vec3(max.x, min.y, max.z);
	boxPoints[5] = glm::vec3(min.x, max.y, max.z);
	boxPoints[6] = glm::vec3(max.x, max.y, min.z);
	boxPoints[7] = max;
}
bool isCross(float ix, float ax, float oix, float oax)
{
	return (ax < oax && ax > oix);

}
bool AABB::isIntersect(const AABB& other)
{
	bool x_cross =  isCross(min.x, max.x, other.min.x, other.max.x) || isCross(other.min.x, other.max.x, min.x, max.x);
	bool y_cross =  isCross(min.y, max.y, other.min.y, other.max.y) || isCross(other.min.y, other.max.y, min.y, max.y);
	bool z_cross =  isCross(min.z, max.z, other.min.z, other.max.z) || isCross(other.min.z, other.max.z, min.z, max.z);
	return x_cross && y_cross && z_cross;
}

float* AABB::getBoxConPoints() 
{
	return (float *)boxPoints;
}


int* AABB::getBoxTriangleIndices()
{
	return boxIndices;
}

glm::vec3 vec42vec3(glm::vec4 vec)
{
	return glm::vec3(vec.x, vec.y, vec.z);
}

// ------------------------------ MOVEMENT CLASS -------------------------- //
Movement::Movement(glm::vec3 front, glm::mat4 model, float speed)
{
	this->front = glm::normalize(glm::vec4(front, 0));
	this->moveSpeed = speed;
	this->model = model;
}

glm::mat4 Movement::goFront()
{
	model = glm::translate(model, vec42vec3(front * moveSpeed));
	return model;
}

glm::mat4 Movement::goBack()
{
	model = glm::translate(model, vec42vec3(-front * moveSpeed));
	return model;
}

glm::mat4 Movement::goLeft() 
{
	glm::mat4 trans(1.f);
	trans = glm::rotate(trans, glm::radians(90.f), glm::vec3(0, 1.f, 0));
	glm::vec4 newdir4 = trans * front;
	glm::vec3 newdir = glm::vec3(newdir4.x, newdir4.y, newdir4.z);
	model = glm::translate(model, newdir * moveSpeed);
	return model;
}
glm::mat4 Movement::goRight() 
{
	glm::mat4 trans(1.f);
	trans = glm::rotate(trans, glm::radians(-90.f), glm::vec3(0, 1.f, 0));
	glm::vec4 newdir4 = trans * front;
	glm::vec3 newdir = glm::vec3(newdir4.x, newdir4.y, newdir4.z);
	model = glm::translate(model, newdir * moveSpeed);
	return model;
}

glm::mat4 Movement::goUp()
{
	model = glm::translate(model, glm::vec3(0, 1, 0) * moveSpeed);
	return model;
}

glm::mat4 Movement::goDown()
{
	model = glm::translate(model, glm::vec3(0, -1, 0) * moveSpeed);
	return model;
}

void Movement::changeSpeed(float s)
{
	moveSpeed += s;
}


void updateIfnInsect(AABB old, const AABB& other, const glm::mat4& newModel, Movement& oldMove, const Movement& newMove)
{
	glm::vec4 aabbmin(old.min, 1.f);
	glm::vec4 aabbmax(old.max, 1.f);
	AABB newAABB(vec42vec3(newModel * aabbmin), vec42vec3(newModel * aabbmax));
	if (!newAABB.isIntersect(other))
	{
		oldMove = newMove;
	}
}