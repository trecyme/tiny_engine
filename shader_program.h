#ifndef _SHADER_PROGRAME_H
#define _SHADER_PROGRAME_H

typedef unsigned int uint;



class ShadarProgram
{

public:
	virtual void setUniformMat4(const char* name, glm::mat4 mtx) = 0;
	virtual void setUniformFloat(const char* name, float value) = 0;
	virtual void setUniformInt(const char* name, int value) = 0;
	virtual void setUniformVec3(const char* name, glm::vec3 vec) = 0;
	virtual void setUniformMat4Arr(const char* name, glm::mat4* mtx, int n) = 0;
	virtual void use() = 0;
	
};


class OpenGLShadarProgram : ShadarProgram

{

public:
	void setUniformMat4(const char* name, glm::mat4 mtx);
	void setUniformFloat(const char* name, float value);
	void setUniformInt(const char* name, int value);
	void setUniformVec3(const char* name, glm::vec3 vec);
	void setUniformMat4Arr(const char* name, glm::mat4* mtx, int n);
	void use();
	void query();
	OpenGLShadarProgram(const char* vsrc, const char* fsrc);
	OpenGLShadarProgram(const char* vsrc, const char* fsrc, const char* gsrc);
	~OpenGLShadarProgram();

private:
	uint pgID;
	uint vsID;
	uint gsID;
	uint fsID;
};

#endif // !_SHADER_PROGRAME_H