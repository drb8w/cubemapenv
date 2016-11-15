#pragma once

/** @file FrameBufferObjectView.h
*@brief frame buffer object view class file.
*/

#include "TextureView.h"

/** Uses the bound texture of the given OpenGL frame buffer object as the buffer to be viewed.
*/
class FrameBufferObjectView : public TextureView
{

private:
	GLuint m_fboID;

public:

	/** Constructor.
	* @param width width of buffer
	* @param height height of buffer
	* @param windowName basic name part of OpenCV windows for viewing the buffer
	* @param channels identifies the channel combination that should be shown (in several windows)
	* @param fboID identifies the frame buffer object that will be bound
	* @param textureUnitID identifies the textureUnit that will be activated
	* @param textureID identifies the texture that should be bound
	*/
	FrameBufferObjectView(int width, int height, const std::string &windowName, Channels channels, GLuint fboID, GLuint textureUnitID, GLuint textureID, GLenum textureTarget = GL_TEXTURE_2D);

protected:
	virtual cv::Mat ReadBufferToMatrix();
};