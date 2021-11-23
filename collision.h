#ifndef _COLLISION_H
#define _COLLISION_H

#include "common.h"

/*
	碰撞检测类，AABB方法
*/
class AABB {
private:
	glm::vec3 boxPoints[8];
	int boxIndices[36]{
		0, 1, 2,
		0, 2, 3,
		4, 5, 7,
		4, 6, 7,

		0, 1, 5,
		5, 4, 0,
		1, 2, 6,
		6, 5, 1,

		3, 2, 7,
		2, 7, 6,
		0, 3, 7,
		7, 4, 0
	};
	void updateBoxPoint();

public:
	glm::vec3 min;
	glm::vec3 max;

	AABB();
	AABB(glm::vec3 min, glm::vec3 max);
	AABB(FbxVector4* controlPoints, int n);
	
	
	void set(glm::vec3 min, glm::vec3 max);
	void update(glm::mat4 mtx);
	bool isIntersect(const AABB& other);
	float* getBoxConPoints();
	int* getBoxTriangleIndices();

};


/*
	物体的移动，前后上下移动，左右转动
*/
class Movement
{
private:
	glm::vec4 front;
	float moveSpeed;

public:
	glm::mat4 model;
	Movement(glm::vec3 front, glm::mat4 model, float mspeed);

	glm::mat4 goFront();
	glm::mat4 goBack();
	glm::mat4 goLeft();
	glm::mat4 goRight();
	glm::mat4 goUp();
	glm::mat4 goDown();
	void changeSpeed(float s);

};

void updateIfnInsect(AABB old, const AABB& other, const glm::mat4& newModel, Movement& oldMove, const Movement& newMove);

#endif // !#define _COLLISION_H
