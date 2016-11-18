#include "TextureBoxFramebuffer.h"

#define USE_DEPTH_BUFFER
//#define USE_DEFAULT_BUFFER_OBJECT
//#define USE_DEFUALT_MVC

namespace TotalGlobal
{

	// http://www.mbroecker.com/project_dynamic_cubemaps.html
	// OpenGL Superbible 7, Off-Screen rendering
	bool TextureBoxFramebuffer::CreateFBOCubemap()
	{
		// create a frame buffer object
		// ----------------------------------------------------------------
#ifdef USE_DEFAULT_BUFFER_OBJECT
		m_Index_fbo = 0;
#else
		glGenFramebuffers(1, &m_Index_fbo);
		// TODO: reuse this framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, m_Index_fbo); // bind generated framebuffer for writing, i.e. rendering
#endif
		// cretate the cubemap, i.e. the color texture for our frame buffer
		// ----------------------------------------------------------------
		// version 2.0 - multiple texture units
		glActiveTexture(GL_TEXTURE0 + m_Index_texUnit);
		glGenTextures(1, &m_Index_color_texture);
		glBindTexture(GL_TEXTURE_2D, m_Index_color_texture);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_Width, m_Height);
		
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// no mipmaps so turn them off for the texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

#ifdef USE_DEPTH_BUFFER
		// cretate the cubemap, i.e. the depth texture for our depth buffer
		// ----------------------------------------------------------------
		// version 2.0 - multiple texture units
		glActiveTexture(GL_TEXTURE0 + m_Index_texUnit);  // is this valid to bind it to the same texture unit? Does this unbind color_texture?
		glGenTextures(1, &m_Index_depth_texture);
		glBindTexture(GL_TEXTURE_2D, m_Index_depth_texture);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, m_Width, m_Height);
#endif

		// attach color and depth texture to the framebuffer
		// ----------------------------------------------------------------
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_Index_color_texture, 0);
#ifdef USE_DEPTH_BUFFER
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_Index_depth_texture, 0);
#endif

		// test framebuffer status
		GLenum status_fbo = glCheckFramebufferStatus(m_Index_fbo); // GL_FRAMEBUFFER_COMPLETE = 0x8CD5 i.e. 36053
		// GL_INVALID_ENUM is generated if target is not GL_FRAMEBUFFER.

		// disable
#ifndef USE_DEFAULT_BUFFER_OBJECT
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}

	bool TextureBoxFramebuffer::Render()
	{
		// bind the fbo, save our OpenGL state
		BeginRendering();

		// ----------------------------------------------------------------------------------------------
		// 1.st render pass: render the cubemap itself into the skybox
		// ----------------------------------------------------------------------------------------------

		static const GLfloat gray[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		static const GLfloat ones[] = { 1.0f };
		//const float t = (float)currentTime * 0.1f;
		const float t = 0.0f;

		vmath::mat4 proj_matrix = vmath::perspective(60.0f, (float)m_Width / (float)m_Height, 0.1f, 1000.0f);
		vmath::mat4 view_matrix = vmath::lookat(vmath::vec3(15.0f * sinf(t), 0.0f, 15.0f * cosf(t)),
			vmath::vec3(0.0f, 0.0f, 0.0f),
			vmath::vec3(0.0f, 1.0f, 0.0f));
		vmath::mat4 mv_matrix = view_matrix *
			vmath::rotate(t, 1.0f, 0.0f, 0.0f) *
			vmath::rotate(t * 130.1f, 0.0f, 1.0f, 0.0f) *
			vmath::translate(0.0f, -4.0f, 0.0f);

		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, ones);

		glBindTexture(GL_TEXTURE_CUBE_MAP, m_Index_tex_envmap);

		glViewport(0, 0, m_Width, m_Height);

		glUseProgram(m_Index_skybox_prog);
		glBindVertexArray(m_Index_skybox_vao);

		glUniformMatrix4fv(m_Index_skybox_view_matrix, 1, GL_FALSE, view_matrix);

		glDisable(GL_DEPTH_TEST);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// ----------------------------------------------------------------------------------------------
		// 2.nd render pass: render the content of the cubemap without the object that use the cubemap
		// ----------------------------------------------------------------------------------------------

		// TEST
		for (int i = 0; i < 6; ++i)
		{
			// attach the correct cube face, clear it and set up the matrices
			// and draw the scene without certain objects
			DrawToFace(i);
		}
		//DrawToFace(0);

		// unbind fbo, return to backbuffer rendering, cubemap is now ready to use
		EndRendering();

		return true;
	}

	// bind the fbo, save our OpenGL state
	bool TextureBoxFramebuffer::BeginRendering()
	{
#ifndef USE_DEFAULT_BUFFER_OBJECT
		glBindFramebuffer(GL_FRAMEBUFFER, m_Index_fbo); // bind generated framebuffer for writing, i.e. rendering
		static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, draw_buffers);

		glActiveTexture(GL_TEXTURE0 + m_Index_texUnit);
		glBindTexture(GL_TEXTURE_2D, m_Index_color_texture);  // necessary  because of call to glFramebufferTexture2D later ???
#ifdef USE_DEPTH_BUFFER
		glBindTexture(GL_TEXTURE_2D, m_Index_depth_texture);   // necessary and correct to bind to the same texture unit ???
#endif

#endif
		return true;
	}
	
	// http://www.mbroecker.com/project_dynamic_cubemaps.html
	// OpenGL Superbible 7, Off-Screen rendering
	// attach the correct cube face, clear it and set up the matrices
	bool TextureBoxFramebuffer::DrawToFace(GLint index_face)
	{
		//// iterate over this variable six times, once for each face. See below
		//int face = 0;

#ifndef USE_DEFAULT_BUFFER_OBJECT
		// attach new texture and renderbuffer to fbo
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Index_color_texture, 0);

#ifdef USE_DEPTH_BUFFER
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_Index_depth_texture, 0); // ???
#endif

#endif
		//verifyStatus();

		// clear
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glMatrixMode(GL_PROJECTION);
		//glLoadIdentity();

		//// play around with the near and far values
		//gluPerspective(90, 1, 1, 1000);
		vmath::mat4 proj_matrix = vmath::perspective(90.0f, (float)m_Width / (float)m_Height, 0.1f, 1000.0f);

		//glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity();

		vmath::mat4 view_matrix;

		// setup lookat depending on current face
		switch (GL_TEXTURE_CUBE_MAP_POSITIVE_X + index_face)
		{
		case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
			//gluLookAt(0, 0, 0, 1, 0, 0, 0, 1, 0);
			view_matrix = vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(1.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 0.0f, -1.0f));
			break;

		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
			//gluLookAt(0, 0, 0, -1, 0, 0, 0, 1, 0);
			view_matrix = vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(-1.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 0.0f, 1.0f));
			break;

		case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
			//gluLookAt(0, 0, 0, 0, 10, 0, 1, 0, 0);
			view_matrix = vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f), vmath::vec3(-1.0f, 0.0f, 0.0f));
			break;

		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
			//gluLookAt(0, 0, 0, 0, -10, 0, 1, 0, 0);
			view_matrix = vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, -1.0f, 0.0f), vmath::vec3(1.0f, 0.0f, 0.0f));
			break;

		case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
			//gluLookAt(0, 0, 0, 0, 0, 10, 0, 1, 0);
			view_matrix = vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 0.0f, 1.0f), vmath::vec3(0.0f, -1.0f, 0.0f));
			break;

		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
			//gluLookAt(0, 0, 0, 0, 0, -10, 0, 1, 0);
			view_matrix = vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 0.0f, -1.0f), vmath::vec3(0.0f, 1.0f, 0.0f));
			break;

		default:
			break;
		};

		// glTranslatef(-renderPosition.x, -renderPosition.y, -renderPosition.z);
		vmath::mat4 mv_matrix = view_matrix *
			vmath::translate(-m_RenderPosition[0], -m_RenderPosition[1], -m_RenderPosition[2]);

		// ready to draw here
		// -------------------------
		static const GLfloat gray[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		static const GLfloat blue[] = { 0.2f, 0.2f, 0.8f, 1.0f };
		static const GLfloat ones[] = { 1.0f };
		glClearBufferfv(GL_COLOR, m_Index_fbo, blue);
#ifdef USE_DEPTH_BUFFER
		glClearBufferfv(GL_DEPTH, m_Index_fbo, ones);
#endif
		glViewport(0, 0, m_Width, m_Height);

		glUseProgram(m_Index_cubemap_render_prog);

#ifdef USE_DEFUALT_MVC
		//const float t = (float)currentTime * 0.1f;
		const float t = 0.0f;

		proj_matrix = vmath::perspective(60.0f, (float)m_Width / (float)m_Height, 0.1f, 1000.0f);
		view_matrix = vmath::lookat(vmath::vec3(15.0f * sinf(t), 0.0f, 15.0f * cosf(t)),
			vmath::vec3(0.0f, 0.0f, 0.0f),
			vmath::vec3(0.0f, 1.0f, 0.0f));
		mv_matrix = view_matrix *
			vmath::rotate(t, 1.0f, 0.0f, 0.0f) *
			vmath::rotate(t * 130.1f, 0.0f, 1.0f, 0.0f) *
			vmath::translate(0.0f, -4.0f, 0.0f);
#endif

		glUniformMatrix4fv(m_Index_cubemap_render_mv_matrix, 1, GL_FALSE, mv_matrix);
		glUniformMatrix4fv(m_Index_cubemap_render_proj_matrix, 1, GL_FALSE, proj_matrix);

#ifdef USE_DEPTH_BUFFER
		glEnable(GL_DEPTH_TEST);
#endif

		for (auto iter = m_Objects.begin(); iter != m_Objects.end(); ++iter)
		{
			sb7::object *pObject = *iter;
			pObject->render();
		}

		return true;
	}


	// unbind fbo, return to backbuffer rendering, cubemap is now ready to use
	bool TextureBoxFramebuffer::EndRendering()
	{
#ifndef USE_DEFAULT_BUFFER_OBJECT
		glBindTexture(GL_TEXTURE_2D, 0); // unbind the cube map
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind the frame buffer
#endif
		return true;
	}


}