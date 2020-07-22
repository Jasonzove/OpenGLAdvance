#include <windows.h>
#include "shader_coder.h"

std::map<int, std::string> ShaderCoder::mShaders;

std::string ShaderCoder::Get(const int & shaderId)
{
	if (mShaders.find(shaderId) != mShaders.end())
	{
		return mShaders[shaderId];
	}

	HMODULE hModule = ::GetModuleHandle(TEXT("OpenGLAdvance.exe"));

	const char* data;
	int size;
	HRSRC rc = ::FindResource(hModule, MAKEINTRESOURCE(shaderId), TEXT("SHADER_FILE"));
	if (rc == nullptr)
	{
		printf("Shader::Get(): rc == nullptr!\n");
		return "";
	}

	HGLOBAL rcData = ::LoadResource(hModule, rc);
	size = ::SizeofResource(hModule, rc);
	data = static_cast<const char*>(::LockResource(rcData));
	mShaders[shaderId] = std::string(data, size);
	return mShaders[shaderId];
}
