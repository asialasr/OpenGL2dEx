#include "shader.h"

#include "logging.h"
#include "gl_debug.h"
#include <fstream>
#include <sstream>

namespace util {

namespace {
enum class CompileErrorCheckType {
	kProgram = 0,
	kLinker,
	kNumTypes,
	kUnknown,
};
void check_compile_errors(unsigned int object, CompileErrorCheckType type, const util::Optional<int> shader_type)
{
	static constexpr size_t kInfoLogLength{ 1024 };
	char info_log[kInfoLogLength];
	int success{};

	switch (type)
	{
	case CompileErrorCheckType::kProgram:
	{
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(object, kInfoLogLength, NULL, info_log);
			// TODO(sasiala): ASSERT(false, "Shader compilation failed.  Shader type: " + std::to_string(shader_type) + ". Info log: " + std::string{ info_log });
			ASSERT(false, "Shader compilation failed. Info log: " + std::string{ info_log });
		}
	}
	break;
	case CompileErrorCheckType::kLinker:
	{
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(object, kInfoLogLength, NULL, info_log);
			// TODO(sasiala): improve logging
			ASSERT(false, "Shader linking failed.  Info log: " + std::string{ info_log });
		}
	}
	break;
	default:
		ASSERT(false, "Unknown compile error checking type");
		break;
	}

	check_for_gl_errors();
}

std::string shader_code(const char *shader_path)
{
	std::ifstream file{};
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	std::stringstream ss{};
	try
	{
		file.open(shader_path);

		ss << file.rdbuf();
		file.close();
	}
	catch (std::ifstream::failure e)
	{
		ASSERT(false, "Failure in reading shader source file");
	}

	return ss.str();
}

unsigned int compile_shader(const char * const shader_code, int shader_type)
{
	// fragment shader
	unsigned int shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &shader_code, NULL);
	glCompileShader(shader);

	check_compile_errors(shader, CompileErrorCheckType::kProgram, {shader_type});

	return shader;
}
} // namespace

Shader::Shader(const char *vertex_path, const char *fragment_path, Optional<const char *> geometry_path)
{
	std::string vertex_code{ "" };
	std::string geometry_code{ "" };
	std::string fragment_code{ "" };

	vertex_code = shader_code(vertex_path);
	if (geometry_path)
	{
		geometry_code = shader_code(*geometry_path);
	}
	fragment_code = shader_code(fragment_path);

	// compile shader
	unsigned int vertex_shader_id{}, geometry_shader_id{}, fragment_shader_id{};
	vertex_shader_id = compile_shader(vertex_code.c_str(), GL_VERTEX_SHADER);
	if (geometry_path)
	{
		geometry_shader_id = compile_shader(geometry_code.c_str(), GL_GEOMETRY_SHADER);
	}
	fragment_shader_id = compile_shader(fragment_code.c_str(), GL_FRAGMENT_SHADER);

	id_ = glCreateProgram();
	glAttachShader(id_, vertex_shader_id);
	if (geometry_path)
	{
		glAttachShader(id_, geometry_shader_id);
	}
	glAttachShader(id_, fragment_shader_id);

	glLinkProgram(id_);

	check_compile_errors(id_, CompileErrorCheckType::kLinker, {});

	// delete the shaders since they're already linked in
	glDeleteShader(vertex_shader_id);
	if (geometry_path)
	{
		glDeleteShader(geometry_shader_id);
	}
	glDeleteShader(fragment_shader_id);
}

void Shader::use() const
{
	glUseProgram(id_);
	check_for_gl_errors();
}

namespace {
	unsigned int uniform_location(const unsigned int id, const std::string &name, bool allow_invalid)
	{
		const auto uniform_loc = glGetUniformLocation(id, name.c_str());
		if (!allow_invalid)
		{
			// TODO(sasiala): improve logging (add var name)
			static constexpr int kGetUniformLocationError{ -1 };
			ASSERT(uniform_loc != kGetUniformLocationError, "Error: variable not found.  Var name: " + name);
		}

		return uniform_loc;
	}
}


void Shader::set_bool(const std::string &name, const bool value, const bool allow_invalid) const
{
	glUniform1i(uniform_location(id_, name, allow_invalid), static_cast<int>(value));
	check_for_gl_errors();
}

void Shader::set_int(const std::string &name, const int value, const bool allow_invalid) const
{
	glUniform1i(uniform_location(id_, name, allow_invalid), value);
	check_for_gl_errors();
}

void Shader::set_float(const std::string &name, const float value, const bool allow_invalid) const
{
	glUniform1f(uniform_location(id_, name, allow_invalid), value);
	check_for_gl_errors();
}

void Shader::set_vec2(const std::string &name, const float val_1, const float val_2, const bool allow_Invalid) const
{
	glUniform2f(uniform_location(id_, name, allow_Invalid), val_1, val_2);
	check_for_gl_errors();
}

void Shader::set_vec2(const std::string &name, const glm::vec2 &vec, const bool allow_invalid) const
{
	set_vec2(name, vec.x, vec.y, allow_invalid);
	check_for_gl_errors();
}

void Shader::set_vec3(const std::string &name, const float val_1, const float val_2, const float val_3, const bool allow_invalid) const
{
	glUniform3f(uniform_location(id_, name, allow_invalid), val_1, val_2, val_3);
	check_for_gl_errors();
}

void Shader::set_vec3(const std::string &name, const glm::vec3 &vec, bool allow_invalid) const
{
	set_vec3(name, vec.x, vec.y, vec.z, allow_invalid);
	check_for_gl_errors();
}

void Shader::set_vec4(const std::string &name, float val_1, float val_2, float val_3, float val_4, bool allow_invalid) const
{
	glUniform4f(uniform_location(id_, name, allow_invalid), val_1, val_2, val_3, val_4);
	check_for_gl_errors();
}

void Shader::set_vec4(const std::string &name, const glm::vec4 &vec, bool allow_invalid) const
{
	set_vec4(name, vec.x, vec.y, vec.z, vec.w, allow_invalid);
	check_for_gl_errors();
}

void Shader::set_mat2(const std::string &name, const glm::mat2 &mat, const bool allow_invalid) const
{
	glUniformMatrix2fv(uniform_location(id_, name, allow_invalid), 1, GL_FALSE, &mat[0][0]);
	check_for_gl_errors();
}

void Shader::set_mat3(const std::string &name, const glm::mat3 &mat, const bool allow_invalid) const
{
	glUniformMatrix3fv(uniform_location(id_, name, allow_invalid), 1, GL_FALSE, &mat[0][0]);
	check_for_gl_errors();
}

void Shader::set_mat4(const std::string &name, const glm::mat4 &mat, const bool allow_invalid) const
{
	glUniformMatrix4fv(uniform_location(id_, name, allow_invalid), 1, GL_FALSE, &mat[0][0]);
	check_for_gl_errors();
}

} // namespace util