#include "common.h"
#include "shader_program.h"


void OpenGLShadarProgram::setUniformMat4(const char* name, glm::mat4 mtx)
{
	int loc = glGetUniformLocation(pgID, name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mtx));

}
void OpenGLShadarProgram::setUniformMat4Arr(const char* name, glm::mat4* mtx, int n)
{
	int loc = glGetUniformLocation(pgID, name);
	glUniformMatrix4fv(loc, n, GL_FALSE, (float *)mtx);

}
void OpenGLShadarProgram::setUniformInt(const char * name, int value)
{
	int loc = glGetUniformLocation(pgID, name);
	glUniform1i(loc, value);
}
void OpenGLShadarProgram::setUniformFloat(const char* name, float value)
{
	int loc = glGetUniformLocation(pgID, name);
	glUniform1f(loc, value);
}
void OpenGLShadarProgram::setUniformVec3(const char* name, glm::vec3 vec)
{
	int loc = glGetUniformLocation(pgID, name);
	glUniform3fv(loc, 1, glm::value_ptr(vec));
}

void OpenGLShadarProgram::use()
{
	glUseProgram(pgID);
}

void OpenGLShadarProgram::query()
{
	int nActAttr;
	glGetProgramiv(pgID, GL_ACTIVE_ATTRIBUTES, &nActAttr);
	std::cout << "n active attr - " << nActAttr << std::endl;
	const int n = 50;
	char name[n];
	int size;
	int len;
	GLenum type;
	for (int i = 0; i < nActAttr; i++)
	{
		glGetActiveAttrib(pgID, i, n, &len, &size, &type, name);
		std::cout << i << " active attr - " << name  
			<< " - " << type
			<< std::endl;

	}
}

OpenGLShadarProgram::OpenGLShadarProgram(const char* vsrc, const char* fsrc) 
{
	vsID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vsID, 1, &vsrc, NULL);
	glCompileShader(vsID);
	int  success;
	char infoLog[512];
	glGetShaderiv(vsID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vsID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	fsID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fsID, 1, &fsrc, NULL);
	glCompileShader(fsID);
	glGetShaderiv(fsID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fsID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	pgID = glCreateProgram();
	glAttachShader(pgID, vsID);
	glAttachShader(pgID, fsID);
	glLinkProgram(pgID);

	glGetProgramiv(pgID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(pgID, 512, NULL, infoLog);
		std::cout << "ERROR::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
	}
}
OpenGLShadarProgram::OpenGLShadarProgram(const char* vsrc, const char* fsrc, const char* gsrc)
{
	vsID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vsID, 1, &vsrc, NULL);
	glCompileShader(vsID);
	int  success;
	char infoLog[512];
	glGetShaderiv(vsID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vsID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	fsID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fsID, 1, &fsrc, NULL);
	glCompileShader(fsID);
	glGetShaderiv(fsID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fsID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	
	gsID = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(gsID, 1, &gsrc, NULL);
	glCompileShader(gsID);
	glGetShaderiv(gsID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(gsID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	pgID = glCreateProgram();
	glAttachShader(pgID, vsID);
	glAttachShader(pgID, fsID);
	glAttachShader(pgID, gsID);
	glLinkProgram(pgID);

	glGetProgramiv(pgID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(pgID, 512, NULL, infoLog);
		std::cout << "ERROR::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
	}
}


OpenGLShadarProgram::~OpenGLShadarProgram()
{
	glDeleteShader(vsID);
	glDeleteShader(fsID);
	glDeleteProgram(pgID);
}
