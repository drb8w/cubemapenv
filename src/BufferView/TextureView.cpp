#include "TextureView.h"

TextureView::TextureView(int width, int height, const std::string &windowName, Channels channels, GLuint textureUnitID, GLuint textureID, GLenum textureTarget) 
	: BufferView(width, height, windowName, channels), m_textureUnitID(textureUnitID), m_textureID(textureID), m_textureTarget(textureTarget)
{
	char _Dest2[99];
	_itoa(textureUnitID, _Dest2, 10);
	std::string texUnitIDStr = std::string(_Dest2);

	char _Dest[99];
	_itoa(textureID, _Dest, 10);
	std::string texIDStr = std::string(_Dest);

	m_bufferWindowName = "texture view: " + windowName + " tex unit: " + texUnitIDStr + " tex: " + texIDStr;

	m_texBuffer = new GLubyte[m_width*m_height*3];
	for (int i = 0; i < m_width*m_height*3; i++)
		m_texBuffer[i] = 0;

	m_texMat = cv::Mat(m_height, m_width, CV_8UC3, m_texBuffer);
}

cv::Mat TextureView::ReadBufferToMatrix()
{
	// TODO: backup current used texture

	// activate corresponding texture unit for read?
	glActiveTexture(GL_TEXTURE0 + m_textureUnitID);
	// bind texture
	glBindTexture(m_textureTarget, m_textureID);
	// get texture information
	glGetTexImage(m_textureTarget, 0, GL_RGB, GL_UNSIGNED_BYTE, m_texBuffer);
	// TODO: restore texture
	/*glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);*/

	// make an OpenCV image
	cv::Mat texMat(m_height, m_width, CV_8UC3, m_texBuffer);
	// is this necessary?
	texMat.copyTo(m_texMat);
	//cv::flip(depthMat, m_depthMat, 0);
	//cv::cvtColor(m_RGBMat, m_BGRMat, CV_RGB2BGR);

	return m_texMat;
}

//void printPixelsOfMipMap(int mipMapLevel)   
//{   
//	//GLint textureWidth, textureHeight;
//	//glGetTexLevelParameter(GL_TEXTURE_2D, mipMapLevel, GL_TEXTURE_WIDTH, &textureWidth);
//	//glGetTexLevelParameter(GL_TEXTURE_2D, mipMapLevel, GL_TEXTURE_HEIGHT, &textureHeight);
//	//GLubyte *m_texBuffer = new GLubyte[textureWidth*textureHeight * 3];
//
//	////glGenTextures(1, &tex1);
//	////glBindTexture(GL_TEXTURE_2D, tex1);
//	////glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
//
//	//glGetTexImage(GL_TEXTURE_2D, mipMapLevel, GL_RGB, GL_UNSIGNED_BYTE, buffer);
//	//for (int y = 0; y < textureHeight; y++)
//	//for (int x = 0; x < textureWidth; x++)
//	//{
//	//	int startAddressOfPixel = ((y*textureWidth) + x) * 4;
//	//	printf("pixel at %d %d has colour r=%d g=%d b=%d a=%d\n", x, y, buffer[startAddressOfPixel], buffer[startAddressOfPixel + 1], buffer[startAddressOfPixel + 2], startAddressOfPixel[startAddressOfPixel + 3]);
//	//}
//	//free(buffer);
//}