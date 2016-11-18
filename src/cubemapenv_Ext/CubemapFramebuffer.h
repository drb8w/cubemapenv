#pragma once

#include <sb7.h>
#include <vmath.h>

#include <object.h>
#include <shader.h>
#include <sb7ktx.h>

#include <vector>

namespace TotalGlobal
{

	class CubemapFramebuffer
	{
	private:

		// Input
		GLuint m_Index_skybox_prog;
		GLuint m_Index_texunit_envmap;
		GLuint m_Index_tex_envmap;

		GLuint m_Index_cubemap_render_prog;

		GLsizei m_Width = 512;
		GLsizei m_Height = 512;

		// output
		GLuint m_Index_fbo;

		GLuint m_Index_color_texture;
		GLuint m_Index_depth_texture;

		GLuint m_Index_texUnit = 2;
		GLuint m_Index_sampler;

		// intermediate

		GLuint m_Index_skybox_vao;

		vmath::vec3 m_RenderPosition = vmath::vec3(0.0f, 0.0f, 0.0f);

		// uniforms
		GLint  m_Index_skybox_view_matrix;

		GLint m_Index_cubemap_render_mv_matrix;
		GLint m_Index_cubemap_render_proj_matrix;

		// object list
		std::vector<sb7::object *> m_Objects;

	public:
		CubemapFramebuffer(GLuint index_skybox_prog,
			GLint index_texunit_envmap,
			GLuint index_tex_envmap,
			GLuint index_cubemap_render_prog,
			const std::vector<sb7::object *> &objects, GLsizei width = 512, GLsizei height = 512)
			: m_Index_skybox_prog(index_skybox_prog),
			m_Index_texunit_envmap(index_texunit_envmap),
			m_Index_tex_envmap(index_tex_envmap),
			m_Index_cubemap_render_prog(index_cubemap_render_prog),
			m_Objects(objects), m_Width(width), m_Height(height)
		{
			CreateFBOCubemap();

			glGenVertexArrays(1, &m_Index_skybox_vao);
			glBindVertexArray(m_Index_skybox_vao);

			m_Index_skybox_view_matrix = glGetUniformLocation(index_skybox_prog, "view_matrix");
			m_Index_cubemap_render_mv_matrix = glGetUniformLocation(index_cubemap_render_prog, "mv_matrix");
			m_Index_cubemap_render_proj_matrix = glGetUniformLocation(index_cubemap_render_prog, "proj_matrix");
		}

		bool Render();

		GLuint GetFramebufferIndex()
		{
			return m_Index_fbo;
		}

		GLuint GetTextureUnitIndex()
		{
			return m_Index_texUnit;
		}

		GLuint GetCubeMapColorTextureIndex()
		{
			return m_Index_color_texture;
		}

	private:

		bool CreateFBOCubemap();

		// bind the fbo, save our OpenGL state
		bool BeginRendering();		

		// attach the correct cube face, clear it and set up the matrices
		bool DrawToFace(GLint index_face);

		//bool DrawScene();

		// unbind fbo, return to backbuffer rendering, cubemap is now ready to use
		bool EndRendering();
	
	};


}

