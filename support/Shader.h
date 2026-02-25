#pragma once

#include <support/WinInclude.h>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <string_view>

class Shader
{
public:
	Shader();
	Shader(std::string_view name);
	~Shader();

	inline const void* GetBuffer() const { return m_data; }
	inline size_t GetSize() const { return m_size; }

public:
	void LoadFromFile(std::string_view fileName);

private:
	void Load();

private:
	std::string_view m_name;
	void* m_data = nullptr;
	size_t m_size = 0;
};

