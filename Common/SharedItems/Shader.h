#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

class ShaderProgramSource;
class Shader
{
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	// set uniforms
	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform1b(const std::string& name, bool value);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

	// Getter
	inline unsigned int GetID() const { return m_RendererID; }
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	std::unordered_map<std::string, int> m_UniformLoactionCache;
	int GetUniformLocation(const std::string& name);
};

