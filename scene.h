#ifndef SCENE_H
#define SCENE_H
#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include<algorithm>
#include "tools.h"

using namespace cv;
using namespace std;

#define SI_NUM_THRESHOLD 10

typedef struct FrameInfo_s
{
	Mat _m256;
	int _pos;
	int _rgbHist[64];
	int _ts_ms;
} FrameInfo;

typedef struct SceneInfo_s
{
	int _start;
	int _end;
	int _starttime_ms, _endtime_ms;
	int _framenum;
	int _mh_score;
	//Mat _m256_mean;
	//int _mhash[64];
	int _rgbHist[64];
	int _struct_fp[48 * 48];
	float _cosSimi_mean;
} SceneInfo;

bool comp_byscore(const FrameInfo &a, const FrameInfo &b);
bool comp_bypos(const FrameInfo &a, const FrameInfo &b);
bool comp_byfn(const SceneInfo &a, const SceneInfo &b);
bool comp_by_starttime(const SceneInfo &a, const SceneInfo &b);
void saveSI(string &basename, SceneInfo &si, int id, vector<FrameInfo> &fis);

void calcFeature_grayH(SceneInfo &si, vector<FrameInfo> &fis);
void calcFeature_structfp(SceneInfo &si, vector<FrameInfo> &fis);
void calcFeature_mhash(SceneInfo &si, vector<FrameInfo> &fis);

#endif // SCENE_H
