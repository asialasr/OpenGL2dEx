#version 330 core
in vec2 io_tex_coords_;
out vec4 io_color_;

uniform sampler2D u_image_;
uniform vec3 u_sprite_color_;

void main()
{
	io_color_ = vec4(u_sprite_color_, 1.0) * texture(u_image_, io_tex_coords_);
}