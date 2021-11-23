#ifndef LIGHT_H
#define LIGHT_H

#include "common.h"


struct Light
{
	enum EType {
		eParallel,
		eSpot,
		eFlash
	};
	EType etype;

	Light(glm::vec3 pos, glm::vec3 dir, glm::vec3 color) 
		: pos(pos), color(color), dir(dir), etype(Light::eParallel) {}
	Light(glm::vec3 pos, glm::vec3 color, float c, float l, float q) 
		: pos(pos), color(color), constant(c), liner(l), quadratic(q), etype(Light::eSpot) {}
	Light(glm::vec3 pos, glm::vec3 color, float c, float l, float q, float cf, float ocf) :
		pos(pos), color(color), constant(c), liner(l), quadratic(q), cutoff(cf), outerCutoff(ocf), etype(Light::eFlash) {}
	
	/*Light(EType etype, glm::vec3 pos, glm::vec3 dir, glm::vec3 color, float c, float l, float q, float cf, float ocf) :
		etype(etype),pos(pos), dir(dir), color(color), constant(c), liner(l), quadratic(q), cutoff(cf), outerCutoff(ocf) {}*/

	
	glm::vec3 pos;
	glm::vec3 dir;
	glm::vec3 color;

	// Ë¥¼õ
	float constant;
	float liner;
	float quadratic;

	// ¾Û¹â
	float cutoff;
	float outerCutoff;
};

#endif // !LIGHT_H
