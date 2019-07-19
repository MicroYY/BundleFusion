#pragma once

#include "GlobalAppState.h"

#ifdef TCP_SENSOR

#include <WinSock2.h>

#include "RGBDSensor.h"

#pragma comment(lib, "ws2_32.lib")

class TCPSensor :public RGBDSensor
{
public:
	TCPSensor();

	~TCPSensor();

	void createFirstConnected();

	bool processDepth();

	bool processColor();

	std::string getSensorName()const {
		return "TCPSensor";	
	}

private:

	unsigned int colorWidth, colorHeight, depthWidth, depthHeight;
	unsigned int bufSize;

	char* recvImg;
	uchar* recvImgRGB;
	uchar* recvImgDepth;
	

	SOCKET clientSocket;
};



#endif // TCP_SENSOR
