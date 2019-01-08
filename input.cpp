
#include "input.h"


CInput::CInput()
{
	_url = NULL;
	_timestamp_ms = -1;
	_width = 0, _height = 0;

}

CInput::~CInput()
{

}


CInputVideo::CInputVideo()
{
	_timestamp_ms = -1;
	_bLoops = 0;
	_nDuration_ms = -1;
}

CInputVideo::~CInputVideo()
{
}

int CInputVideo::Open(char *url)
{
	_url = url;
	if (_url == NULL)
		return -1;

	_cap.open(_url);
	if (!_cap.isOpened())
	{
		cout << " video file [" << _url << "] open failed! " << endl;
		return -1;
	}

	float fps = _cap.get(CV_CAP_PROP_FPS);
	float framenum = _cap.get(CV_CAP_PROP_FRAME_COUNT);
	if (fps <= 0)
		return -1;
	_nDuration_ms = (framenum / fps) * 1000;
	//cout << "video duration : " << _nDuration_ms << endl;
	_nBaseTimestamp_ms = 0;

	_width = int(_cap.get(CV_CAP_PROP_FRAME_WIDTH));
	_height = int(_cap.get(CV_CAP_PROP_FRAME_HEIGHT));

	return 0;
}

int CInputVideo::Open(char *url, int bLoops)
{
	_bLoops = bLoops;
	return Open(url);
}

int CInputVideo::Get_FirstFrame(Mat &firstImg)
{
	if (!_matFirst.empty())
	{
		firstImg = _matFirst;
		return 0;
	}

	VideoCapture cap(_url);
	if (!cap.isOpened())
		return -1;
	cap >> _matFirst;
	firstImg = _matFirst;

	return 0;
}

int CInputVideo::Get_Duration()
{
    return _nDuration_ms;
}

float CInputVideo::Get_FrameRate()
{
	return _cap.get(CV_CAP_PROP_FPS);
}

int CInputVideo::Close()
{
	return 0;
}

int CInputVideo::Get_CurrentMat(Mat &img, int startTime, int endTime, int currentTime)
{
	int hopeTimestamp_ms = currentTime - startTime - _nBaseTimestamp_ms;
	if (hopeTimestamp_ms<0)
		return -1;

	while (1)
	{
		if (_timestamp_ms >= hopeTimestamp_ms)
		{
			if (_matCurrent.empty())
			{
				cout << "video _matCurrent is emtpy " << endl;
				return -1;
			}
			img = _matCurrent;
			//cout << _timestamp_ms << endl;
			break;
		}
		else
		{
			_timestamp_ms = _cap.get(CV_CAP_PROP_POS_MSEC);
			_cap >> _matCurrent;
			if (_matCurrent.empty())
			{
				if (_timestamp_ms < 0 || _bLoops == 0)
					return -1;

				// loops == 1
				_nBaseTimestamp_ms += (_timestamp_ms + 1);
				hopeTimestamp_ms = currentTime - startTime - _nBaseTimestamp_ms;
				_timestamp_ms = -1;
				//_cap.open(_url);
				_cap.set(CV_CAP_PROP_POS_MSEC, 0);
				_timestamp_ms = _cap.get(CV_CAP_PROP_POS_MSEC);
				_cap >> _matCurrent;
				continue;
			}
		}
	}
	return 0;
}
