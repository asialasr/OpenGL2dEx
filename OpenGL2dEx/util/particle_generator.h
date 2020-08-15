#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H

#include "optional.h"
#include "shader.h"
#include "texture_2d.h"

#include <glm/glm.hpp>
#include <vector>

namespace util {

class GameObject;

class ParticleGenerator {
public:
	struct Particle {
		Particle()
			: position_{ 0.0f }
			, velocity_{ 0.0f }
			, color_{ 1.0f }
			, life_{ 0.0f }
		{
		}

		glm::vec2 position_;
		glm::vec2 velocity_;
		glm::vec4 color_;
		float     life_;
	}; // class Particle

	ParticleGenerator(Shader shader, Texture2D texture, size_t max_particles, const glm::mat4 &projection)
		: shader_{ shader }
		, texture_{ texture }
		, max_particles_{ max_particles }
		, particles_(max_particles, Particle{})
		, unused_vertices_{}
		, vao_{}
	{
		initialize(projection);
	}

	void update(float dt, GameObject &object, unsigned int new_particles, Optional<glm::vec2> offset);
	void draw();

private:
	size_t pop_vertex()
	{
		const auto vertex = unused_vertices_.back();
		unused_vertices_.pop_back();
		return vertex;
	}

	void initialize(const glm::mat4 &projection);

	Shader shader_;
	Texture2D texture_;

	const size_t		  max_particles_;
	std::vector<Particle> particles_;
	std::vector<size_t>   unused_vertices_;

	unsigned int vao_;
}; // class ParticleGenerator

} // namespace util

#endif // !PARTICLE_GENERATOR_H
