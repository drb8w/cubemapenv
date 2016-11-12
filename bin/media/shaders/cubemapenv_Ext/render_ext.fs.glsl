#version 410 core

//uniform (binding = 2) samplerCube tex_cubemap;
//uniform (binding = 0) samplerCube tex_cubemap;
uniform samplerCube tex_cubemap;

uniform sampler2D texSampler_test; // get this uniform on CPU-side by name, i.e. glGetUniformLocation

in VS_OUT
{
    vec3 normal;
    vec3 view;
} fs_in;

out vec4 color;

void main(void)
{
    // Reflect view vector about the plane defined by the normal
    // at the fragment
    vec3 r = reflect(fs_in.view, normalize(fs_in.normal));

    // Sample from scaled using reflection vector
	color = texture(tex_cubemap, r) *vec4(0.95, 0.80, 0.45, 1.0);
}
