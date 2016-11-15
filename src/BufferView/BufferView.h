#pragma once

/** @file BufferView.h
*@brief Buffer view base class file.
*/

// Standard
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#include "StringHelpers.hpp"
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\nonfree\features2d.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\nonfree\nonfree.hpp>

#include <Windows.h> 
#include <opencv2\highgui\highgui_c.h>

//#include <glew.h>
#include <sb7.h>

/** Channels that indicate how the RGB context should be split up. 
  */
enum Channels {RGB, RG_B, R_G_B, R, G, B};

/** Buffer view base class.
  * Provides OpenCV Window for given buffer.
  * splits up buffer in several channels if desired.
*/
class BufferView{

protected:
	std::string m_bufferWindowName = "buffer view";
	std::vector<std::tuple<HWND, std::string>> m_bufferWindowHandles;

	int m_width = 1024;
	int m_height = 800;

	Channels m_channels;

public:
	/** Constructor.
	* @param width width of buffer
	* @param height height of buffer
	* @param windowName basic name part of OpenCV windows for viewing the buffer
	* @param channels identifies the channel combination that should be shown (in several windows)
	*/
	BufferView(int width, int height, const std::string &windowName, Channels channels = Channels::RGB) : m_width(width), m_height(height), m_bufferWindowName(windowName), m_channels(channels)
	{
		m_bufferWindowName = "buffer view: " + windowName;
	}

	/** Open OpenCV windows with specialized views on given buffer.
	* @param show opens OpenCV windows it true, close existing windows if false
	* @return true if operation was successful
	*/
	virtual bool ShowBufferView(bool show);

	/** Update already opened OpenCV windows with specialized views on given buffer.
	* @return true if operation was successful
	*/
	virtual bool UpdateBufferView();

protected:
	/** Read the given matrix into an OpenCV matrix.
	* @return return the RGB matrix.
	*/
	virtual cv::Mat ReadBufferToMatrix() = 0;

	/** Split the given RGB OpenCV matrix into its channels.
	* @param rgbMat the RGB OpenCV matrix to split
	* @return vector of OpenCV matrix and channel name pairs
	*/
	virtual std::vector<std::tuple<cv::Mat, std::string>> SplitMatrixToChannels(const cv::Mat &rgbMat);
	
	/** Get the name of the OpenCV buffer window for given channels.
	* @param channelName the name identifier of the given channels
	* @return name of OpenCV buffer window 
	*/
	virtual std::string GetBufferWindowChannelName(const std::string &channelName);

};