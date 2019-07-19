#pragma

#include "GlobalAppState.h"


#ifdef MYNTEYE

#include "RGBDSensor.h"

#include <mynteyed/camera.h>
#include <mynteyed/utils.h>
#include <opencv2/opencv.hpp>

class MynteyeSensor :public RGBDSensor
{
public:
	MynteyeSensor();

	~MynteyeSensor();

	void createFirstConnected();

	bool processDepth();

	bool processColor();

	std::string getSensorName() const
	{
		return "MynteyeSensor";
	}

	const unsigned int getFrameNum()
	{
		return frameNum;
	}

private:
	unsigned int colorWidth, colorHeight, depthWidth, depthHeight;

	mynteyed::Camera cam;
	mynteyed::DeviceInfo dev_info;

	unsigned int frameNum;
};

#endif // MYNTEYE
