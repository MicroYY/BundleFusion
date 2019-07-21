#include "stdafx.h"

#include "TCPSensor.h"


#ifdef TCP_SENSOR

TCPSensor::TCPSensor()
{
#ifdef HD

#elif VGA	
	colorWidth = 640;
	colorHeight = 480;

	depthWidth = 640;
	depthHeight = 480;

	RGBDSensor::init(depthWidth, depthHeight, depthWidth, depthHeight, 1);

	initializeDepthIntrinsics(518.52905273437500000, 518.34594726562500000, 319.55499267578125000, 239.06936645507812500);
	initializeDepthExtrinsics(mat4f::identity());

	initializeColorIntrinsics(518.52905273437500000, 518.34594726562500000, 319.55499267578125000, 239.06936645507812500);
	initializeColorExtrinsics(mat4f::identity());

	

#endif // HD
	bufSize = colorHeight * colorWidth * 2 * 3;

	//recvImg = (char*)malloc(sizeof(char) * bufSize);
	//recvImgRGB = (uchar*)recvImg;
	//recvImgDepth = (uchar*)recvImg + bufSize / 2;

	TCHAR* szName = TEXT("imageData");
	HANDLE hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(unsigned char) * bufSize,
		szName);
	recvImg = (unsigned char*)MapViewOfFile(
		hMapFile,
		FILE_MAP_WRITE,
		0,
		0,
		sizeof(unsigned char) * bufSize);
	recvImgRGB = recvImg;
	recvImgDepth = recvImg + bufSize / 2;

#ifdef VR_DISPLAY
	TCHAR vrProcess[] = TEXT("x64\\Release\\VR.exe");
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi;
	DWORD dwExitCode;
	auto iRet = CreateProcess(vrProcess, NULL, NULL, NULL, false, NULL, NULL, NULL, &si, &pi);
	if (iRet)
	{
		std::cout << "VR display process started." << std::endl
			<< "Process ID:\t"
			<< pi.dwProcessId << std::endl;
	}
	else
	{
		std::cout << "Cannot start process!" << std::endl
			<< "Error code:\t" << GetLastError() << std::endl;
	}
#endif // VR_DISPLAY
}

TCPSensor::~TCPSensor()
{
	closesocket(clientSocket);
	WSACleanup();
	free(recvImg);
}

void TCPSensor::createFirstConnected()
{
	WSADATA wsaData;
	int iRet = 0;
	iRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iRet != 0)
	{
		std::cout << "WSAStartup(MAKEWORD(2, 2), &wsaData) execute failed!" << std::endl;
		return;
	}

	if (2 != LOBYTE(wsaData.wVersion) || 2 != HIBYTE(wsaData.wVersion))
	{
		WSACleanup();
		std::cout << "WSADATA version is not correct!" << std::endl;
		return;
	}

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cout << "clientSocket = socket(AF_INET, SOCK_STREAM, 0) execute failed!" << std::endl;
		return;
	}

	SOCKADDR_IN srvAddr;
	srvAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(8088);

	iRet = connect(clientSocket, (SOCKADDR*)&srvAddr, sizeof(SOCKADDR));
	if (iRet != 0)
	{
		std::cout << "connect(clientSocket, (SOCKADDR*)&srvAddr, sizeof(SOCKADDR)) execute failed!" << std::endl;
		return;
	}
	printf("连接已建立");

	char sendBuf = 0xa5;
	send(clientSocket, &sendBuf, 1, 0);

	return;
}

bool TCPSensor::processDepth()
{
	int iRet = recv(clientSocket, (char*)recvImg, bufSize, 0);
	while (iRet != bufSize)
	{
		iRet += recv(clientSocket, (char*)(recvImg) + iRet, bufSize - iRet, 0);
	}


	for (int i = 0; i < colorHeight; i++)
	{
		for (size_t j = 0; j < colorWidth; j++)
		{
			unsigned int index = i * colorWidth + j;
			m_colorRGBX[index].z = recvImgRGB[index * 3 + 0];
			m_colorRGBX[index].y = recvImgRGB[index * 3 + 1];
			m_colorRGBX[index].x = recvImgRGB[index * 3 + 2];
			m_colorRGBX[index].w = 255;
		}
	}

	float* depth = getDepthFloat();
	for (size_t i = 0; i < depthHeight; i++)
	{
		for (size_t j = 0; j < depthWidth; j++)
		{
			unsigned int index = i * depthWidth + j;
			const unsigned short& d = (recvImgDepth[index * 3 + 0] << 8) | recvImgDepth[index * 3 + 1];
			if (d == 0)
			{
				depth[index] = -std::numeric_limits<float>::infinity();
			}
			else
			{
				depth[index] = (float)d * 0.001f;
			}
		}
	}

	return true;
}

bool TCPSensor::processColor()
{
	/*int iRet = recv(clientSocket, recvImg, bufSize / 2, 0);
	while (iRet != bufSize / 2)
	{
		iRet += recv(clientSocket, &recvImg[iRet], bufSize / 2 - iRet, 0);
	}


	float* depth = getDepthFloat();

	for (size_t i = 0; i < depthHeight; i++)
	{
		for (size_t j = 0; j < depthWidth; j++)
		{
			unsigned int index = i * depthWidth + j;
			const unsigned short& d = (recvImg[index * 3 + 0] << 8) | recvImg[index * 3 + 1];
			if (d == 0)
			{
				depth[index] = -std::numeric_limits<float>::infinity();
			}
			else
			{
				depth[index] = (float)d * 0.001f;
			}
		}
	}*/
	return true;
}


#endif // TCP_SENSOR
