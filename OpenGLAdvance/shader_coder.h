#pragma once
#include <map>
#include <string>

class ShaderCoder
{
public:
	static std::string Get(const int& shaderId);
private:
	static std::map<int, std::string> mShaders;
};