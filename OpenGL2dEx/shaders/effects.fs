#version 330 core
in vec2 io_tex_coords_;
out vec4 io_color_;

uniform sampler2D u_scene_;
uniform vec2      u_offsets_[9];
uniform int       u_edge_kernel_[9];
uniform float     u_blur_kernel_[9];

uniform bool u_chaos_;
uniform bool u_confuse_;
uniform bool u_shake_;

void main()
{
	io_color_ = vec4(0.0);
	vec3 sample[9];

	if (u_chaos_ || u_shake_)
	{
		for (int i = 0; i < 9; i++)
		{
			sample[i] = vec3(texture(u_scene_, io_tex_coords_.st + u_offsets_[i]));
		}
	}

	if (u_chaos_)
	{
		for (int i = 0; i < 9; i++)
		{
			io_color_ += vec4(sample[i] * u_edge_kernel_[i], 0.0);
		}
		io_color_.a = 1.0;
	}
	else if (u_confuse_)
	{
		io_color_ = vec4(1.0 - texture(u_scene_, io_tex_coords_).rgb, 1.0);
	}
	else if (u_shake_)
	{
		for (int i = 0; i < 9; i++)
		{
			io_color_ += vec4(sample[i] * u_blur_kernel_[i], 0.0);
		}
		io_color_.a = 1.0;
	}
	else
	{
		io_color_ = texture(u_scene_, io_tex_coords_);
	}
}