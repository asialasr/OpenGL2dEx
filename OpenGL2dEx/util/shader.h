#ifndef SHADER_H
#define SHADER_H
#include "optional.h"
#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace util {

	class Shader {
	public:
		Shader(const Shader& other)
			: id_{ other.id_ }
		{
		}

		Shader(const char *vertex_path, const char *fragment_path, Optional<const char *> geometry_path);

		void use() const
		{
			glUseProgram(id_);
		}

		void set_bool(const std::string &name, bool value, bool allow_invalid) const;
		void set_int(const std::string &name, int value, bool allow_invalid) const;
		void set_float(const std::string &name, float value, bool allow_invalid) const;
		void set_vec2(const std::string &name, float val_1, float val_2, bool allow_Invalid) const;
		void set_vec2(const std::string &name, const glm::vec2 &vec, bool allow_invalid) const;
		void set_vec3(const std::string &name, float val_1, float val_2, float val_3, bool allow_invalid) const;
		void set_vec3(const std::string &name, const glm::vec3 &vec, bool allow_invalid) const;
		void set_mat2(const std::string &name, const glm::mat2 &mat, bool allow_invalid) const;
		void set_mat3(const std::string &name, const glm::mat3 &mat, bool allow_invalid) const;
		void set_mat4(const std::string &name, const glm::mat4 &mat, bool allow_invalid) const;

	private:
		unsigned int id_;
	};

} // namespace util

#endif // SHADER_H