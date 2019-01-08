#include <fstream>
#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include<algorithm>
#include "scene.h"
#include "CalcHistogram.h"
#include "input.h"

using namespace cv;
using namespace std;

static CalcHistogram g_hist;
static const int g_cSceneNum = 10;
static const int g_cSceneScoreThres = 56;
static string g_szBasename;

void scanVFile(vector<FrameInfo> &fis, char *vfile)
{
	VideoCapture cap;
	cap.open(vfile);
	if (!cap.isOpened())
	{
		//cout << " cap err" << endl;
		return;
	}

	int count = 0;
	Mat imgSrc;
	while (1)
	{
		double timestamp_ms = cap.get(CV_CAP_PROP_POS_MSEC);
		cap >> imgSrc;
		if (imgSrc.empty())
		{
			break;
		}

		FrameInfo fi;
		preprocessMat2(imgSrc, fi._m256);
		fi._pos = count;
		fi._ts_ms = int(timestamp_ms);

		g_hist.mat2RGBHist(fi._m256, fi._rgbHist);

		fis.push_back(fi);

		if (count == 432211)
		{
			//imwrite(".\\11\\9-4322.jpg", fi._m256);
			//exit(0);
		}
			
		count++;
	}
}

void fis2sis(vector<FrameInfo> &fis, vector<SceneInfo> &sis_out)
{
	int start = 0;
	
	vector<SceneInfo> sis;
	float fSumCosSimi = 0;
	for (int i = 0; i < fis.size(); i++)
	{
		if (i == 0)
		{
			continue;
		}

		float fCosSimi_frames = calcCosSimi(fis[i-1]._rgbHist, fis[i]._rgbHist);
		float fCosSimi_start_end = calcCosSimi(fis[start]._rgbHist, fis[i]._rgbHist);
		fSumCosSimi += fCosSimi_frames;

		//cout << "cos simi : " << fCosSimi_frames << " " << fCosSimi_start_end << endl;
		if (fCosSimi_frames < 0.99 || fCosSimi_start_end<0.92)
		{
			//cout << " ************* !!!!" << fis[i]._ts_ms << endl;
			fSumCosSimi -= fCosSimi_frames;

			SceneInfo si;
			int end = i - 1;
			si._start = start;
			si._end = end;
			si._starttime_ms = fis[start]._ts_ms;
			si._endtime_ms = fis[end]._ts_ms;
			si._framenum = end - start + 1;
			si._mh_score = 1;
			if (si._framenum == 1)
				si._cosSimi_mean = 0;
			else
				si._cosSimi_mean = fSumCosSimi / (si._framenum - 1);
			sis.push_back(si);
			start = i;
			fSumCosSimi = 0;
		}
	}

	SceneInfo si;
	si._start = start;
	si._end = fis.size() - 1;
	si._starttime_ms = fis[start]._ts_ms;
	si._endtime_ms = fis[si._end]._ts_ms;
	si._framenum = si._end - si._start + 1;
	if (si._framenum == 1)
		si._cosSimi_mean = 0;
	else
		si._cosSimi_mean = fSumCosSimi / (si._framenum - 1);
	sis.push_back(si);

	// ÅÅÐò³¡¾°
	sort(sis.begin(), sis.end(), comp_byfn);
	//cout << "scene size : " << sis.size() << endl;

	
	for (int i = 0; i < sis.size(); i++)
	{
		int framenum = sis[i]._end - sis[i]._start;
		if (framenum<12 && i>SI_NUM_THRESHOLD)
			continue;

		sis_out.push_back(sis[i]);
		

		//calcFeature_grayH(sis[i], fis);
		//calcFeature_structfp(sis[i], fis);

		//calcFeature_mhash(sis[i], fis);
		//imshow("mean", sis[i]._m256_mean);
		
		//if (sis[i]._start - 1>0)
		//	imshow("sum1", fis[sis[i]._start-1]._m256);
		
		//imshow("sum", fis[sis[i]._start]._m256);
		//cout << sis[i]._framenum << endl;
		//waitKey();
	}

	sort(sis_out.begin(), sis_out.end(), comp_by_starttime);
}

int output_sis(vector<SceneInfo> &sis, vector<FrameInfo> &fis, char *szVFile, char *szCsvfile, char *szWorkpath)
{
	if (szCsvfile == NULL || szVFile == NULL)
		return -1;

	CInputVideo *pVideo;
	pVideo = new CInputVideo();
	if (pVideo->Open(szVFile) != 0)
		return -1;

	string workpath = szWorkpath;
	ofstream csvFile(szCsvfile);
	for (int i = 0; i < sis.size(); i++)
	{
		string sz_num;
		int2string(sz_num, i);

		csvFile << sis[i]._starttime_ms << "," << sis[i]._endtime_ms << ",";
		for (int count = 0; count < 3; count++)
		{
			string sz_ss_count;
			int2string(sz_ss_count, count);
			string snapshot = workpath + "/scene-" + sz_num + "-" + sz_ss_count + ".jpg";

			int offset = 160;
			if (sis[i]._endtime_ms - sis[i]._starttime_ms > 1000)
				offset = (sis[i]._endtime_ms - sis[i]._starttime_ms)/4;
			//cout << offset << endl;
			Mat img;
			if (count == 0)
				pVideo->Get_CurrentMat(img, 0, 1000000, sis[i]._starttime_ms + offset);
			else if (count==1)
				pVideo->Get_CurrentMat(img, 0, 1000000, (sis[i]._starttime_ms+sis[i]._endtime_ms)/2);
			else
				pVideo->Get_CurrentMat(img, 0, 1000000, sis[i]._endtime_ms - offset);
			imwrite(snapshot, img);
			csvFile << snapshot;
			if (count != 2)
				csvFile << ",";
			else
				csvFile << "\n";
		}
		
	}

	return 0;
}

int get_vsfp(char *vfile, char *szCsvfile, char *szWorkpath)
{
	vector<FrameInfo> fis;
	scanVFile(fis, vfile);
	if (fis.size() == 0)
		return -1;

	vector<SceneInfo> sis;
	fis2sis(fis, sis);

	int ret;
	ret = output_sis(sis, fis, vfile, szCsvfile, szWorkpath);

	return ret;
}

int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		cout << "err: no argv [v-sfp vfile csvfile workpath]" << endl;
		return -1;
	}

	//getThumbnailBaseName(g_szBasename, argv[1], argv[2]);
	return get_vsfp(argv[1], argv[2], argv[3]);
}
