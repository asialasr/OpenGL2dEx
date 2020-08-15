#version 330 core

in vec2 io_tex_coords_;
out vec4 io_color_;

uniform vec4 u_color_;
uniform sampler2D u_sprite_;

void main()
{
	io_color_ = (texture(u_sprite_, io_tex_coords_) * u_color_);
}