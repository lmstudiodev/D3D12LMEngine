#include "Shader.h"

Shader::Shader() : m_name("")
{
}

Shader::Shader(std::string_view name) : m_name(name)
{
	Load();
}

Shader::~Shader()
{
	if(m_data)
		free(m_data);
}

void Shader::LoadFromFile(std::string_view fileName)
{
	m_name = fileName;

	Load();
}

void Shader::Load()
{
	static std::filesystem::path shaderDir;

	if (shaderDir.empty())
	{
		wchar_t moduleFileName[MAX_PATH];

		GetModuleFileNameW(nullptr, moduleFileName, MAX_PATH);

		shaderDir = moduleFileName;
		shaderDir.remove_filename();
	}

	std::ifstream shaderIn(shaderDir / m_name, std::ios::binary);

	if (shaderIn.is_open())
	{
		shaderIn.seekg(0, std::ios::end);
		m_size = shaderIn.tellg();
		shaderIn.seekg(0, std::ios::beg);
		m_data = malloc(m_size);

		if (m_data)
		{
			shaderIn.read((char*)m_data, m_size);
		}
	}
	else
	{
		std::cout << "[D3D12] Shader File " << m_name << " not found !!!" << std::endl;
	}

	std::cout << "[D3D12] Shader File " << m_name << " loaded !!!" << std::endl;
}
