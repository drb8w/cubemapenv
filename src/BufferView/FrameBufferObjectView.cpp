#include "FrameBufferObjectView.h"


FrameBufferObjectView::FrameBufferObjectView(int width, int height, const std::string &windowName, Channels channels, GLuint fboID, GLuint textureUnitID, GLuint textureID, GLenum textureTarget)
	: TextureView(width, height, windowName, channels, textureUnitID, textureID, textureTarget), m_fboID(fboID)
{
	char _Dest3[99];
	_itoa(fboID, _Dest3, 10);
	std::string fboIDStr = std::string(_Dest3);
	
	char _Dest2[99];
	_itoa(textureUnitID, _Dest2, 10);
	std::string texUnitIDStr = std::string(_Dest2);

	char _Dest[99];
	_itoa(textureID, _Dest, 10);
	std::string texIDStr = std::string(_Dest);

	m_bufferWindowName = "frame buffer object view: " + windowName + " fbo: " + fboIDStr + " tex unit: " + texUnitIDStr + " tex: " + texIDStr;

}

cv::Mat FrameBufferObjectView::ReadBufferToMatrix()
{
	// TODO: backup current used fbo

	// bind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
	
	cv::Mat mat = TextureView::ReadBufferToMatrix();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return mat;
}
