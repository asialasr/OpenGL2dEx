#version 330 core
layout (location = 0) in vec4 l_vertex_; // <vec2 position, vec2 tex_coords>

out vec2 io_tex_coords_;

uniform mat4 u_projection_;
uniform vec2 u_offset_;

void main()
{
	float scale = 10.0f;
	io_tex_coords_ = l_vertex_.zw;
	gl_Position = u_projection_ * vec4((l_vertex_.xy * scale) + u_offset_, 0.0, 1.0);
}