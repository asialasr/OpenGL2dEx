#version 330 core
layout (location = 0) in vec4 l_vertex_; // <vec2 position, vec2 tex_coords>

out vec2 io_tex_coords_;

uniform mat4 u_model_;
uniform mat4 u_projection_;

void main()
{
	io_tex_coords_ = l_vertex_.zw;
	gl_Position = u_projection_ * u_model_ * vec4(l_vertex_.xy, 0.0, 1.0);
}