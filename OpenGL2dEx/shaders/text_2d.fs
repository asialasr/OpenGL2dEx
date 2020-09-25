#version 330 core
in vec2 io_tex_coords_;
out vec4 io_color_;

uniform sampler2D u_text_;
uniform vec3      u_text_color_;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_text_, io_tex_coords_).r);
	io_color_ = vec4(u_text_color_, 1.0) * sampled;
}