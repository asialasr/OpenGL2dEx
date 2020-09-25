#version 330 core
layout (location = 0) in vec4 l_vertex_; // <vec2 pos, vec2 tex>
out vec2 io_tex_coords_;

uniform mat4 u_projection_;

void main()
{
	gl_Position = u_projection_ * vec4(l_vertex_.xy, 0.0, 1.0);
	io_tex_coords_ = l_vertex_.zw;
}