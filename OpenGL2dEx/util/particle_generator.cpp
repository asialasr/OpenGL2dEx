#include "particle_generator.h"

#include "gl_debug.h"

#include "game_object.h"
#include "resource_mgr.h"

namespace util {

namespace {
	void respawn_particle(ParticleGenerator::Particle &particle, 
						  GameObject &object, 
						  const glm::vec2 &offset)
	{
		float random = ((rand() % 100) - 50) / 10.0f;
		float r_color = 0.5f + ((rand() % 100) / 100.0f);
		particle.position_ = object.position() + random + offset;
		particle.color_ = glm::vec4(r_color, r_color, r_color, 1.0f);
		particle.life_ = 1.0f;
		particle.velocity_ = object.velocity() * 0.1f;
	}
} // namespace

void ParticleGenerator::update(float dt, GameObject &object, unsigned int new_particles, Optional<glm::vec2> offset)
{
	for (size_t i = 0; i < new_particles; ++i)
	{
		respawn_particle(particles_.at(pop_vertex()), object, (offset) ? *offset : glm::vec2(0.0f, 0.0f));
	}

	for (size_t i = 0; i < particles_.size(); ++i)
	{
		auto &particle = particles_.at(i);
		if (particle.life_ > 0.0f)
		{
			particle.life_ -= dt;
			if (particle.life_ <= 0.0f)
			{
				unused_vertices_.push_back(i);
			}
		}

		if (particle.life_ > 0.0f)
		{
			particle.position_ -= particle.velocity_ * dt;
			particle.color_.a -= dt * 2.5f;
		}
	}
}

void ParticleGenerator::draw()
{
	// use additive blending to give it a "glow" effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	shader_.use();
	for (auto &particle : particles_)
	{
		if (particle.life_ > 0.0f)
		{
			shader_.set_vec2("u_offset_", particle.position_, false);
			shader_.set_vec4("u_color_", particle.color_, false);
			texture_.bind();

			glBindVertexArray(vao_);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}

	// reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	check_for_gl_errors();
}

void ParticleGenerator::initialize(const glm::mat4 &projection)
{
	for (size_t i = 0; i < max_particles_; ++i)
	{
		unused_vertices_.push_back(i);
	}

	unsigned int vbo;
	static constexpr float particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &vao_);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao_);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	shader_.use();
	shader_.set_int("u_sprite_", 0, false);
	shader_.set_mat4("u_projection_", projection, false);

	check_for_gl_errors();
}

} // namespace util