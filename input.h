#ifndef INPUT_H
#define INPUT_H

#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;

class CInput
{
public:
	CInput();
	virtual ~CInput();

public:
	virtual int Open(char *url) = 0;
	virtual int Close() = 0;
	virtual int Get_FirstFrame(Mat &firstImg) = 0;
	int Get_Width() { return _width; }
	int Get_Height() { return _height; }

	// notice: 这里是 浅拷贝，请不要修改 img 的数据
	virtual int Get_CurrentMat(Mat &img, int startTime, int endTime, int currentTime) = 0;

protected:
	char *_url;
	int _timestamp_ms;
	Mat _matCurrent, _matFirst;
	int _width, _height;
};

class CInputVideo : public CInput
{
public:
	CInputVideo();
	~CInputVideo();

public:
	int Open(char *url);
	int Open(char *url, int bLoops);
	int Close();

	int Get_FirstFrame(Mat &firstImg);
	int Get_CurrentMat(Mat &img, int startTime, int endTime, int currentTime);
    int Get_Duration();
	float Get_FrameRate();

private:
	VideoCapture _cap;
	int _bLoops;
	int _nDuration_ms;
	int _nBaseTimestamp_ms;
};

#endif // INPUT_H
