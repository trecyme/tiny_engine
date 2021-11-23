#ifndef _UTIL_H
#define _UTIL_H

#include "common.h"
#include <codecvt>

void printFloatArr(float* arr, int len);
void printDoubleArr(double* arr, int len);
void printIntArr(int* arr, int len);

/*
	从文件中读取字符串
*/
std::string read_string_of_file(const char* path);

/*
	将字符串转换为UTF-8编码的字符串
*/
std::wstring conv_utf8(std::string str);

/*
	当前时间（毫秒级）
*/
long int nowSecond();

/*
	硬编码函数，为了那些不规范命名的纹理文件
*/
std::string fixTextureFileName(const char* filename);

/*
	glm:vec3转换为str，FOR DEBUG
*/
std::string vec32str(glm::vec3 vec);

/*
	glm:vec4转换为str，FOR DEBUG
*/
glm::vec3 vec42vec3(glm::vec4);


#endif // !_UTIL_H
