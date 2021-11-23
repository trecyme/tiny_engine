#include "util.h"


/*
	从文件中读取字符串
*/
std::string read_string_of_file(const char* path)
{
	std::cout << "File name:" << path << std::endl;
	std::ifstream fin(path, std::ifstream::in);
	fin.seekg(0, fin.end);
	int count = (int)fin.tellg();
	if (count <= 0)
	{
		std::cout << "ERROR: FAIL!" << std::endl;
		return std::string();
	}

	fin.seekg(0);

	std::cout << "Number of chars: " << count << std::endl;
	std::stringstream sstrm;
	while (!fin.eof())
	{

		std::string line;
		getline(fin, line);
		//std::cout << line << std::endl;
		sstrm << line << std::endl;
	}
	fin.close();
	return sstrm.str();
}

/*
	将字符串转换为UTF-8编码的字符串
*/
std::wstring conv_utf8(std::string str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::wstringstream sstrm;
	std::wstring wb = conv.from_bytes(str);
	sstrm << wb;
	std::wstring wstr = sstrm.str();
	return wstr;
}

/*
	测试函数
*/
int mainxxxx(int argc, char** argv)
{
	std::cout << read_string_of_file("../default.vs");
	std::cout << read_string_of_file("../default.fs");
	std::cout << "end" << std::endl;
	return 0;
}


/*
	输出double数组
*/
void printDoubleArr(double* arr, int len)
{
	for (int i = 0; i < len; i++)
		std::cout << arr[i] << ", ";
	std::cout << std::endl;
}

/*
	输出float数组
*/
void printFloatArr(float* arr, int len)
{
	for (int i = 0; i < len; i++)
		std::cout << arr[i] << ", ";
	std::cout << std::endl;
}

/*
	输出int数组
*/
void printIntArr(int* arr, int len)
{
	for (int i = 0; i < len; i++)
		std::cout << arr[i] << ", ";
	std::cout << std::endl;
}


/*
	当前时间（秒级）
*/
long int nowSecond()
{
	time_t tnow = time(0);
	long int t = static_cast<long int>(tnow);
	return t;
}



/*
	硬编码，纹理路径的特殊处理
*/
std::string fixTextureFileName(const char* filename)
{
	std::string tmp(filename);
	std::string prefix;
	std::string suffix = ".png";
	bool replaceSuffix = false;
	if (tmp.find("bird") != std::string::npos) {
		prefix = std::string("./bird/bird.fbm/");
	}
	else if (tmp.find("danjuan_v02") != std::string::npos) {
		prefix = std::string("./dick_cat2/textures/");
	}
	else if (tmp.find("danjuan_v04") != std::string::npos) {
		prefix = std::string("./danjuannew/texture/");
		if (tmp.find("4\\danjuan_") != std::string::npos)
			tmp = tmp.replace(tmp.find("4\\danjuan_"), 10, "4\\danjuannew_");
		replaceSuffix = true;
	}
	else if (tmp.find("tangyuan_V02") != std::string::npos && tmp.find("cloth02_color") != std::string::npos) {
		return std::string("./tangyuannew/texture/tangyuannew_cloth_color.png");
	}
	else if (tmp.find("tangyuan_V02") != std::string::npos && tmp.find("body_color04") != std::string::npos) {
		return std::string("./tangyuannew/texture/tangyuannew_body_color.png");
	}
	else if (tmp.find("tangyuan_V02") != std::string::npos) {
		prefix = std::string("./tangyuannew/texture/tangyuannew_");
		replaceSuffix = true;
	}

	int i = tmp.size();
	for (; i > 0; i--) {
		if (tmp[i] == '/' || tmp[i] == '\\') break;
	}
	int j = tmp.size();
	for (; j > 0; j--) {
		if (tmp[j] == '.') break;
	}
	std::string result;
	if (!replaceSuffix)
	{
		result = std::string(tmp.data() + i + 1, tmp.data() + tmp.size());
		return prefix + result;
	}
	else
	{
		result = std::string(tmp.data() + i + 1, tmp.data() + j);
		return prefix + result + suffix;
	}

}



