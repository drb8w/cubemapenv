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
#include "FrameBufferObjectView.h"
#include "TextureView.h"

class cubemapenv_app : public sb7::application
{
private:

	GLsizei width = 512;
	GLsizei height = 512;

	// -----------------------------------------
	TotalGlobal::CubemapFramebuffer *m_pCubemapFrameBuffer;

	TextureView *pTexView;
	FrameBufferObjectView *pFBOView;

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

		pFBOView = new FrameBufferObjectView(width, height, "CubemapFramebuffer", Channels::RGB, 
			m_pCubemapFrameBuffer->GetFramebufferIndex(), m_pCubemapFrameBuffer->GetTextureUnitIndex(), m_pCubemapFrameBuffer->GetCubeMapColorTextureIndex(), 
			GL_TEXTURE_CUBE_MAP);
	}

	void RenderCubeMapExt()
	{
		m_pCubemapFrameBuffer->Render();

		tex_envmap = m_pCubemapFrameBuffer->GetCubeMapColorTextureIndex();
		texunit_envmap = m_pCubemapFrameBuffer->GetTextureUnitIndex();

		// TODO: setup binding correctly... / bind later?
		glActiveTexture(GL_TEXTURE0 + texunit_envmap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex_envmap);

		////glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);


		pTexView->ShowBufferView(showBufferView);
		pFBOView->ShowBufferView(showBufferView);

		if (updateBufferView)
		{
			pTexView->UpdateBufferView();
			pFBOView->UpdateBufferView();
		}

	}

    virtual void startup()
    {

		// ---------------------------------------------
		//std::string execPath = Helper::ExecutionPath();
		//glActiveTexture(GL_TEXTURE0); // to put following textures in texture unit 0 ? 
		//glGenSamplers(1, &sampler);
		// ---------------------------------------------

		// TEST
		envmaps[0] = sb7::ktx::file::load("media/textures/envmaps/mountaincube.ktx");
		//envmaps[0] = sb7::ktx::file::load("C:/Superbible_7/sb7code - master/bin/media/textures/envmaps/mountaincube.ktx");
		tex_envmap = envmaps[envmap_index];

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		// https://www.opengl.org/discussion_boards/showthread.php/181374-finding-out-the-active-texture-from-the-texture-unit
		GLint whichID;
		glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &whichID);
		pTexView = new TextureView(width, height, "mountaincube", Channels::RGB, whichID, tex_envmap, GL_TEXTURE_CUBE_MAP);

        object.load("media/objects/dragon.sbm");
		//object.load("C:/Superbible_7/sb7code - master/bin/media/objects/dragon.sbm");
		m_Objects.push_back(&object);

		//object2.load("media/objects/ladybug.sbm");
		//m_Objects.push_back(&object2);

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
		// 1.st render pass: render the cubemap itself into the skybox
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

		glBindTexture(GL_TEXTURE_CUBE_MAP, tex_envmap);

        glViewport(0, 0, info.windowWidth, info.windowHeight);

        glUseProgram(skybox_prog);
        glBindVertexArray(skybox_vao);

        glUniformMatrix4fv(uniforms.skybox.view_matrix, 1, GL_FALSE, view_matrix);

        glDisable(GL_DEPTH_TEST);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// ----------------------------------------------------------------------------------------------
		// 2.nd render pass: render the content of the cubemap without the object that use the cubemap
		// ----------------------------------------------------------------------------------------------
		RenderCubeMapExt();

		// ----------------------------------------------------------------------------------------------
		// 3.rd render pass: render objects with the corresponding skybox applied to the corresponding object
		// ----------------------------------------------------------------------------------------------

        glUseProgram(render_prog);

        glUniformMatrix4fv(uniforms.render.mv_matrix, 1, GL_FALSE, mv_matrix);
        glUniformMatrix4fv(uniforms.render.proj_matrix, 1, GL_FALSE, proj_matrix);

        glEnable(GL_DEPTH_TEST);

		for (auto it = m_Objects.begin(); it != m_Objects.end(); ++it)
		{
			sb7::object *pObject = *it;
			pObject->render();
		}

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
				case 'S':
					showBufferView = !showBufferView;
					break;
				case 'U':
					updateBufferView = !updateBufferView;
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

	bool showBufferView = false;
	bool updateBufferView = false;

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
	sb7::object     object2;

	std::vector<sb7::object *> m_Objects;

    GLuint          skybox_vao;
};

DECLARE_MAIN(cubemapenv_app)
