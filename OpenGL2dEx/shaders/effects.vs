#version 330 core
layout (location = 0) in vec4 l_vertex_; // <vec2 position, vec2 tex_coords>

out vec2 io_tex_coords_;

uniform bool u_chaos_;
uniform bool u_confuse_;
uniform bool u_shake_;
uniform float u_time_;

void main()
{
	gl_Position = vec4(l_vertex_.xy, 0.0, 1.0);
	vec2 texture = l_vertex_.zw;
	if (u_chaos_)
	{
		float strength = 0.3;
		vec2 pos = vec2(texture.x + sin(u_time_) * strength, texture.y + cos(u_time_) * strength);
		io_tex_coords_ = pos;
	}
	else if (u_confuse_)
	{
		io_tex_coords_ = vec2(1.0 - texture.x, 1.0 - texture.y);
	}
	else
	{
		io_tex_coords_ = texture;
	}

	if (u_shake_)
	{
		float strength = 0.01;
		gl_Position.x += cos(u_time_ * 10) * strength;
		gl_Position.y += cos(u_time_ * 15) * strength;
	}
}