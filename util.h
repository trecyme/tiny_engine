#ifndef _UTIL_H
#define _UTIL_H

#include "common.h"
#include <codecvt>

void printFloatArr(float* arr, int len);
void printDoubleArr(double* arr, int len);
void printIntArr(int* arr, int len);

/*
	���ļ��ж�ȡ�ַ���
*/
std::string read_string_of_file(const char* path);

/*
	���ַ���ת��ΪUTF-8������ַ���
*/
std::wstring conv_utf8(std::string str);

/*
	��ǰʱ�䣨���뼶��
*/
long int nowSecond();

/*
	Ӳ���뺯����Ϊ����Щ���淶�����������ļ�
*/
std::string fixTextureFileName(const char* filename);

/*
	glm:vec3ת��Ϊstr��FOR DEBUG
*/
std::string vec32str(glm::vec3 vec);

/*
	glm:vec4ת��Ϊstr��FOR DEBUG
*/
glm::vec3 vec42vec3(glm::vec4);


#endif // !_UTIL_H
