#include "BufferView.h"

bool BufferView::ShowBufferView(bool show)
{
	if (show)
	{
		if (m_bufferWindowHandles.size() == 0)
		{
			cv::Mat texMat = ReadBufferToMatrix();
			std::vector<std::tuple<cv::Mat, std::string>> splittedMatrixVec = SplitMatrixToChannels(texMat);

			for (std::vector<std::tuple<cv::Mat, std::string>>::iterator it = splittedMatrixVec.begin(); it != splittedMatrixVec.end(); ++it)
			{
				std::string bufferWindowChannelName = GetBufferWindowChannelName(std::get<1>(*it));

				cv::namedWindow(bufferWindowChannelName.c_str(), CV_WINDOW_AUTOSIZE);
				cv::imshow(bufferWindowChannelName.c_str(), std::get<0>(*it));
				HWND bufferWindowHandle = (HWND)cvGetWindowHandle(bufferWindowChannelName.c_str());

				m_bufferWindowHandles.push_back(std::tuple<HWND, std::string>(bufferWindowHandle, bufferWindowChannelName));
			}
		}

	}
	else
	{
		if (m_bufferWindowHandles.size() > 0)
		{
			for (std::vector<std::tuple<HWND, std::string>>::iterator it = m_bufferWindowHandles.begin(); it != m_bufferWindowHandles.end(); ++it)
			{
				cvDestroyWindow(std::get<1>(*it).c_str());
			}
			m_bufferWindowHandles.clear();
		}
	}

	return true;
}

bool BufferView::UpdateBufferView()
{
	if (m_bufferWindowHandles.size() > 0)
	{
		std::vector<std::tuple<HWND, std::string>> actBufferWindowHandles;
		for (std::vector<std::tuple<HWND, std::string>>::iterator it = m_bufferWindowHandles.begin(); it != m_bufferWindowHandles.end(); ++it)
		{
			HWND bufferWindowHandle = std::get<0>(*it);

			if (!IsWindowVisible(bufferWindowHandle))
			{
				// destroy handle
				cvDestroyWindow(std::get<1>(*it).c_str());
			}
			else
			{
				actBufferWindowHandles.push_back(*it);
			}

		}

		m_bufferWindowHandles = actBufferWindowHandles;
		if (m_bufferWindowHandles.size() == 0)
			return false;

		cv::Mat texMat = ReadBufferToMatrix();
		std::vector<std::tuple<cv::Mat, std::string>> splittedMatrixVec = SplitMatrixToChannels(texMat);

		for (std::vector<std::tuple<cv::Mat, std::string>>::iterator it = splittedMatrixVec.begin(); it != splittedMatrixVec.end(); ++it)
		{
			std::string bufferWindowChannelName = GetBufferWindowChannelName(std::get<1>(*it));

			for (std::vector<std::tuple<HWND, std::string>>::iterator it2 = m_bufferWindowHandles.begin(); it2 != m_bufferWindowHandles.end(); ++it2)
			{
				if (std::get<1>(*it2) == bufferWindowChannelName)
					cv::imshow(bufferWindowChannelName.c_str(), std::get<0>(*it));
			}

		}

		return true;

	}

	return false;

}

std::vector<std::tuple<cv::Mat,std::string>> BufferView::SplitMatrixToChannels(const cv::Mat &rgbMat)
{
	std::vector<std::tuple<cv::Mat, std::string>> matrixChannelsVec;

	switch (m_channels)
	{
		case Channels::RGB:
			matrixChannelsVec.push_back(std::tuple<cv::Mat, std::string>(rgbMat,"RGB"));
			break;
		case Channels::RG_B:
			{
				// split in matrix of RG values and matrix of B values
				// ---------------------------------------------------
				// http://stackoverflow.com/questions/6699374/access-to-each-separate-channel-in-opencv
				cv::Mat rgMat(rgbMat.rows, rgbMat.cols, CV_8UC3);
				cv::Mat bMat(rgbMat.rows, rgbMat.cols, CV_8UC3);
				// forming an array of matrices is a quite efficient operation,
				// because the matrix data is not copied, only the headers
				cv::Mat out[] = { rgMat, bMat };
				int from_to[] =
				{ 0, 0, 1, 1,
				  2, 3, 2, 4, 2, 5
				};
				mixChannels(&rgbMat, 1, out, 2, from_to, 5);
				matrixChannelsVec.push_back(std::tuple<cv::Mat, std::string>(rgMat, "RG"));
				matrixChannelsVec.push_back(std::tuple<cv::Mat, std::string>(bMat, "B"));
			}
			break;
		case Channels::R_G_B:
			{
				// http://stackoverflow.com/questions/6699374/access-to-each-separate-channel-in-opencv
				cv::Mat rMat(rgbMat.rows, rgbMat.cols, CV_8UC3);
				cv::Mat gMat(rgbMat.rows, rgbMat.cols, CV_8UC3);
				cv::Mat bMat(rgbMat.rows, rgbMat.cols, CV_8UC3);
				// forming an array of matrices is a quite efficient operation,
				// because the matrix data is not copied, only the headers
				cv::Mat out[] = { rMat, gMat, bMat };
				int from_to[] =
				{ 0, 0, 0, 1, 0, 2,
				  1, 0, 1, 1, 1, 2,
				  2, 0, 2, 1, 2, 2
				};
				mixChannels(&rgbMat, 1, out, 3, from_to, 9);
				matrixChannelsVec.push_back(std::tuple<cv::Mat, std::string>(rMat, "R"));
				matrixChannelsVec.push_back(std::tuple<cv::Mat, std::string>(gMat, "G"));
				matrixChannelsVec.push_back(std::tuple<cv::Mat, std::string>(bMat, "B"));
			}
			break;
		case Channels::R:
			{
				// http://stackoverflow.com/questions/6699374/access-to-each-separate-channel-in-opencv
				cv::Mat rMat(rgbMat.rows, rgbMat.cols, CV_8UC3);
				// forming an array of matrices is a quite efficient operation,
				// because the matrix data is not copied, only the headers
				cv::Mat out[] = { rMat };
				int from_to[] =
				{ 0, 0, 0, 1, 0, 2
				};
				mixChannels(&rgbMat, 1, out, 1, from_to, 3);
				matrixChannelsVec.push_back(std::tuple<cv::Mat, std::string>(rMat, "R"));
			}
			break;
		case Channels::B:
			{	
				// http://stackoverflow.com/questions/6699374/access-to-each-separate-channel-in-opencv
				cv::Mat gMat(rgbMat.rows, rgbMat.cols, CV_8UC3);
				// forming an array of matrices is a quite efficient operation,
				// because the matrix data is not copied, only the headers
				cv::Mat out[] = { gMat };
				int from_to[] =
				{ 1, 0, 1, 1, 1, 2
				};
				mixChannels(&rgbMat, 1, out, 1, from_to, 3);
				matrixChannelsVec.push_back(std::tuple<cv::Mat, std::string>(gMat, "G"));
			}
			break;
		case Channels::G:
			{
				// http://stackoverflow.com/questions/6699374/access-to-each-separate-channel-in-opencv
				cv::Mat bMat(rgbMat.rows, rgbMat.cols, CV_8UC3);
				// forming an array of matrices is a quite efficient operation,
				// because the matrix data is not copied, only the headers
				cv::Mat out[] = { bMat };
				int from_to[] =
				{ 2, 0, 2, 1, 2, 2
				};
				mixChannels(&rgbMat, 1, out, 1, from_to, 3);
				matrixChannelsVec.push_back(std::tuple<cv::Mat, std::string>(bMat, "B"));
			}
			break;
	}

	return matrixChannelsVec;
}


std::string BufferView::GetBufferWindowChannelName(const std::string &channelName)
{
	std::string bufferWindowChannelName = m_bufferWindowName + " channel: " + channelName;
	return bufferWindowChannelName;
}