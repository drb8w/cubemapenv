#pragma once

/** @file TextureView.h
*@brief Texture view class file.
*/

#include "BufferView.h"

/** Uses the given OpenGL texture as the buffer to be viewed.  
 */
class TextureView : public BufferView
{

protected:
	GLubyte *m_texBuffer;
	cv::Mat m_texMat;
	GLuint m_textureID;
	GLuint m_textureUnitID;
	GLenum m_textureTarget = GL_TEXTURE_2D;

public:
	/** Constructor.
	  * @param width width of buffer
	  * @param height height of buffer
	  * @param windowName basic name part of OpenCV windows for viewing the buffer
	  * @param channels identifies the channel combination that should be shown (in several windows)
	  * @param textureUnitID identifies the textureUnit that will be activated
	  * @param textureID identifies the texture that should be bound
	  */
	TextureView(int width, int height, const std::string &windowName, Channels channels, GLuint textureUnitID, GLuint textureID, GLenum textureTarget = GL_TEXTURE_2D);

protected:
	virtual cv::Mat ReadBufferToMatrix();
};