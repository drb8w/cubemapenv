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
		//GLuint cubemap = 0;

		//GLsizei width = 512;
		//GLsizei height = 512;

		//GLuint framebuffer = 0;
		//GLuint depthbuffer = 0;

		// ---------------------------------

		GLuint m_Index_cubemap_render_prog;
		
		GLuint m_Index_fbo;

		GLuint m_Index_color_texture;
		GLuint m_Index_depth_texture;

		GLuint m_Index_texUnit = 2;
		GLuint m_Index_sampler;

		GLsizei m_Width = 512;
		GLsizei m_Height = 512;

		vmath::vec3 m_RenderPosition = vmath::vec3(0.0f, 0.0f, 0.0f);

		// uniforms
		GLint m_Index_mv_matrix;
		GLint m_Index_proj_matrix;
		
		// object list
		std::vector<sb7::object *> m_Objects;

	public:
		CubemapFramebuffer(GLuint index_cubemap_render_prog, const std::vector<sb7::object *> &objects, GLsizei width = 512, GLsizei height = 512)
			: m_Index_cubemap_render_prog(index_cubemap_render_prog), m_Objects(objects), m_Width(width), m_Height(height)
		{
			CreateFBOCubemap();
		}

		bool Render();

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

