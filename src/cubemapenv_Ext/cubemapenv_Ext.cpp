/*
 * Copyright © 2012-2015 Graham Sellers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <sb7.h>
#include <vmath.h>

#include <object.h>
#include <shader.h>
#include <sb7ktx.h>

//#include "StringHelpers.hpp"
#include "CubemapFramebuffer.h"

class cubemapenv_app : public sb7::application
{
private:
	GLuint fbo;
	GLuint color_texture;
	GLuint depth_texture;

	GLuint texunit_cubemap = 2;
	GLuint sampler;

	GLsizei width = 512;
	GLsizei height = 512;

	// -----------------------------------------
	TotalGlobal::CubemapFramebuffer *m_pCubemapFrameBuffer;

public:
    cubemapenv_app()
        : envmap_index(0),
          render_prog(0)
    {
    }

protected:
    void init()
    {
        static const char title[] = "OpenGL SuperBible - Cubic Environment Map";

        sb7::application::init();

        memcpy(info.title, title, sizeof(title));
    }

	void SetupCubeMapExt()
	{
		m_pCubemapFrameBuffer = new TotalGlobal::CubemapFramebuffer(cubemap_render_prog, m_Objects, width, height);

		return;

		// ===================================================================================

		// OpenGL 4.5
		//glCreateFramebuffers(1, &fbo);
		glGenFramebuffers(1, &fbo);
		// TODO: reuse this framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fbo); // bind generated framebuffer for writing, i.e. rendering

		// cretate a texture for our color buffer
		//color_texture;
		// version 1.0 - single texture unit
		//glGenTextures(1, &color_texture);
		//glBindTexture(GL_TEXTURE_2D, color_texture);
		
		// version 2.0 - multiple texture units
		glGenTextures(1, &color_texture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, color_texture);
		// OpenGL 4.5
		//glCreateTextures(1, GL_TEXTURE_2D, &color_texture);
		//glBindTextureUnit(texunit_cubemap, color_texture);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

		// no mipmaps so turn them off for the texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// create a texture for our depth buffer
		//depth_texture;
		// version 1.0 - single texture unit
		//glGenTextures(1, &depth_texture);
		//glBindTexture(GL_TEXTURE_2D, depth_texture);

		// version 2.0 - multiple texture units
		glGenTextures(1, &depth_texture);
		glActiveTexture(GL_TEXTURE2);  // is this valid to bind it to the same texture unit? Does this unbind color_texture?
		glBindTexture(GL_TEXTURE_2D, depth_texture);
		// OpenGL 4.5
		//glCreateTextures(1, GL_TEXTURE_2D, &depth_texture);
		//glBindTextureUnit(texunit_cubemap, depth_texture);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, width, height);

		// attach color and depth texture to the framebuffer
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_texture, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);

		// ----------------------------------------------------------

		// map to other program !!!!
		//glUseProgram(render_prog); // necessary ???
		//glGenSamplers(1, &sampler);
		//glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//GLint texUnit = GL_TEXTURE2 - GL_TEXTURE0;
		//GLuint samplerUniform = glGetUniformLocation(render_prog, "texSampler_test");
		//glUniform1i(samplerUniform, texUnit);
		//glUseProgram(0); // necessary ???

	}

	void RenderCubeMapExt()
	{
		m_pCubemapFrameBuffer->Render();

		tex_envmap = m_pCubemapFrameBuffer->GetCubeMapColorTextureIndex();
		texunit_envmap = m_pCubemapFrameBuffer->GetTextureUnitIndex();

		//// TODO: setup binding correctly... / bind later
		//glActiveTexture(GL_TEXTURE0 + texunit_envmap);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, tex_envmap);

		////glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		return;

		// =========================================================================================

		glBindFramebuffer(GL_FRAMEBUFFER, fbo); // bind generated framebuffer for writing, i.e. rendering

		static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, draw_buffers);

		// -------------------------------------------------------------------------------------------
		// set the camera position and orientation
		// -> set the model-view matrix in the shader

		// view in negative z-direction
		// ----------------------------
		vmath::mat4 proj_matrix = vmath::perspective(60.0f, (float)width / (float)height, 0.1f, 1000.0f);
		vmath::mat4 view_matrix = vmath::lookat(vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 0.0f, -1.0f), vmath::vec3(0.0f, 1.0f, 0.0f));
		vmath::mat4 mv_matrix = view_matrix *
			vmath::rotate(0.0f, 1.0f, 0.0f, 0.0f) *
			vmath::rotate(0.0f, 0.0f, 1.0f, 0.0f) *
			vmath::translate(0.0f, -4.0f, 0.0f);

		static const GLfloat gray[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		static const GLfloat ones[] = { 1.0f };
		glClearBufferfv(GL_COLOR, fbo, gray);
		glClearBufferfv(GL_DEPTH, fbo, ones);
		glViewport(0, 0, width, height);

		glUseProgram(cubemap_render_prog);

		glUniformMatrix4fv(uniforms.cubemap_render.mv_matrix, 1, GL_FALSE, mv_matrix);
		glUniformMatrix4fv(uniforms.cubemap_render.proj_matrix, 1, GL_FALSE, proj_matrix);

		glEnable(GL_DEPTH_TEST);

		object.render();

		// -------------------------------------------------------------------------------------------
		glBindTexture(GL_TEXTURE_2D, 0); // unbind the texture
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // set rendering back to default framebuffer

	}

    virtual void startup()
    {

		// ---------------------------------------------
		//std::string execPath = Helper::ExecutionPath();
		//glActiveTexture(GL_TEXTURE0); // to put following textures in texture unit 0 ? 
		//glGenSamplers(1, &sampler);
		// ---------------------------------------------

		// TEST
		//envmaps[0] = sb7::ktx::file::load("media/textures/envmaps/mountaincube.ktx");
		////envmaps[0] = sb7::ktx::file::load("C:/Superbible_7/sb7code - master/bin/media/textures/envmaps/mountaincube.ktx");
		//tex_envmap = envmaps[envmap_index];

  //      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        object.load("media/objects/dragon.sbm");
		//object.load("C:/Superbible_7/sb7code - master/bin/media/objects/dragon.sbm");
	
		m_Objects.push_back(&object);

        load_shaders();

        glGenVertexArrays(1, &skybox_vao);
        glBindVertexArray(skybox_vao);

		// ---------------------------------------------
		SetupCubeMapExt();
		// ---------------------------------------------

        glDepthFunc(GL_LEQUAL);
    }

    virtual void render(double currentTime)
    {
		// ----------------------------------------------------------------------------------------------
		// 1.st render pass: render the content of the cubemap without the object that use the cubemap
		// ----------------------------------------------------------------------------------------------
		RenderCubeMapExt();

		// ----------------------------------------------------------------------------------------------
		// 2.nd render pass: render the cubemap itself into the skybox
		// ----------------------------------------------------------------------------------------------

		static const GLfloat gray[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        static const GLfloat ones[] = { 1.0f };
        const float t = (float)currentTime * 0.1f;

        vmath::mat4 proj_matrix = vmath::perspective(60.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);
        vmath::mat4 view_matrix = vmath::lookat(vmath::vec3(15.0f * sinf(t), 0.0f, 15.0f * cosf(t)),
                                                vmath::vec3(0.0f, 0.0f, 0.0f),
                                                vmath::vec3(0.0f, 1.0f, 0.0f));
        vmath::mat4 mv_matrix = view_matrix *
                                vmath::rotate(t, 1.0f, 0.0f, 0.0f) *
                                vmath::rotate(t * 130.1f, 0.0f, 1.0f, 0.0f) *
                                vmath::translate(0.0f, -4.0f, 0.0f);

        glClearBufferfv(GL_COLOR, 0, gray);
        glClearBufferfv(GL_DEPTH, 0, ones);

		// setup binding correctly...
		glActiveTexture(GL_TEXTURE0 + texunit_envmap);

		glBindTexture(GL_TEXTURE_CUBE_MAP, tex_envmap);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, color_texture);

        glViewport(0, 0, info.windowWidth, info.windowHeight);

        glUseProgram(skybox_prog);
        glBindVertexArray(skybox_vao);

        glUniformMatrix4fv(uniforms.skybox.view_matrix, 1, GL_FALSE, view_matrix);

        glDisable(GL_DEPTH_TEST);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// ----------------------------------------------------------------------------------------------
		// 3.rd render pass: render objects with the corresponding skybox applied to the corresponding object
		// ----------------------------------------------------------------------------------------------

        glUseProgram(render_prog);

        glUniformMatrix4fv(uniforms.render.mv_matrix, 1, GL_FALSE, mv_matrix);
        glUniformMatrix4fv(uniforms.render.proj_matrix, 1, GL_FALSE, proj_matrix);

        glEnable(GL_DEPTH_TEST);

        object.render();

    }

    virtual void shutdown()
    {
        glDeleteProgram(render_prog);
		glDeleteProgram(cubemap_render_prog);
        glDeleteTextures(3, envmaps);
    }

    void load_shaders()
    {
        if (render_prog)
            glDeleteProgram(render_prog);

        GLuint vs, fs;

        vs = sb7::shader::load("media/shaders/cubemapenv_Ext/render_ext.vs.glsl", GL_VERTEX_SHADER);
        fs = sb7::shader::load("media/shaders/cubemapenv_Ext/render_ext.fs.glsl", GL_FRAGMENT_SHADER);

        render_prog = glCreateProgram();
        glAttachShader(render_prog, vs);
        glAttachShader(render_prog, fs);
        glLinkProgram(render_prog);

        glDeleteShader(vs);
        glDeleteShader(fs);

        uniforms.render.mv_matrix = glGetUniformLocation(render_prog, "mv_matrix");
        uniforms.render.proj_matrix = glGetUniformLocation(render_prog, "proj_matrix");

        vs = sb7::shader::load("media/shaders/cubemapenv_Ext/skybox.vs.glsl", GL_VERTEX_SHADER);
        fs = sb7::shader::load("media/shaders/cubemapenv_Ext/skybox.fs.glsl", GL_FRAGMENT_SHADER);

        skybox_prog = glCreateProgram();
        glAttachShader(skybox_prog, vs);
        glAttachShader(skybox_prog, fs);
        glLinkProgram(skybox_prog);

        glDeleteShader(vs);
        glDeleteShader(fs);

        uniforms.skybox.view_matrix = glGetUniformLocation(skybox_prog, "view_matrix");

		// -------------------------------------------------------------------------------

		if (cubemap_render_prog)
			glDeleteProgram(cubemap_render_prog);

		vs = sb7::shader::load("media/shaders/cubemapenv_Ext/cubemap_render.vs.glsl", GL_VERTEX_SHADER);
		fs = sb7::shader::load("media/shaders/cubemapenv_Ext/cubemap_render.fs.glsl", GL_FRAGMENT_SHADER);

		cubemap_render_prog = glCreateProgram();
		glAttachShader(cubemap_render_prog, vs);
		glAttachShader(cubemap_render_prog, fs);
		glLinkProgram(cubemap_render_prog);

		glDeleteShader(vs);
		glDeleteShader(fs);

		uniforms.cubemap_render.mv_matrix = glGetUniformLocation(cubemap_render_prog, "mv_matrix");
		uniforms.cubemap_render.proj_matrix = glGetUniformLocation(cubemap_render_prog, "proj_matrix");


		// -------------------------------------------------------------------------------

    }

    virtual void onKey(int key, int action)
    {
        if (action)
        {
            switch (key)
            {
                case 'R': load_shaders();
                    break;
                case 'E':
                    envmap_index = (envmap_index + 1) % 3;
                    tex_envmap = envmaps[envmap_index];
                    break;
            }
        }
    }

protected:
    GLuint          render_prog;
    GLuint          skybox_prog;
	GLuint          cubemap_render_prog;

	//CBR
	GLuint			texunit_envmap;

    GLuint          tex_envmap;
    GLuint          envmaps[3];
    int             envmap_index;

    struct
    {
        struct
        {
            GLint       mv_matrix;
            GLint       proj_matrix;
        } render;
        struct
        {
            GLint       view_matrix;
        } skybox;
		struct
		{
			GLint       mv_matrix;
			GLint       proj_matrix;
		} cubemap_render;
		
    } uniforms;

    sb7::object     object;
	
	std::vector<sb7::object *> m_Objects;

    GLuint          skybox_vao;
};

DECLARE_MAIN(cubemapenv_app)
