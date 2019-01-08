#include "CalcHistogram.h"
#include "glcm.h"
#include "scene.h"

static CalcHistogram g_ch_handle;
static GLCM g_glcm;

bool comp_bypos(const FrameInfo &a, const FrameInfo &b)
{
	return a._pos < b._pos;
}

bool comp_byfn(const SceneInfo &a, const SceneInfo &b)
{
	return a._framenum > b._framenum;
}

bool comp_by_starttime(const SceneInfo &a, const SceneInfo &b)
{
	return a._starttime_ms < b._starttime_ms;
}

void saveSI(string &basename, SceneInfo &si, int id, vector<FrameInfo> &fis)
{
	string name, strid, str_s, str_e;
	int2string(strid, id);
	int2string(str_s, si._start);
	int2string(str_e, si._end);

	name = basename + "-" + strid + "-" + str_s + "-" + str_e + ".jpg";
	//imwrite( name, fis[si._start]._m256);
	imwrite(name, fis[si._end]._m256);
}

void calcFeature_grayH(SceneInfo &si, vector<FrameInfo> &fis)
{
	g_ch_handle.mat2RGBHist(fis[si._end]._m256, si._rgbHist);
}

void calcFeature_structfp(SceneInfo &si, vector<FrameInfo> &fis)
{
	Mat_2_structfp(fis[si._end]._m256, si._struct_fp);
}

void calcFeature_mhash(SceneInfo &si, vector<FrameInfo> &fis)
{
#if 1
	Mat m_sum(256, 256, CV_32FC3, Scalar::all(0));
	for (int j = si._start; j < si._end + 1; j++)
	{
		addMat(m_sum, fis[j]._m256);
	}
	//m_sum.convertTo()
	m_sum /= si._framenum;
	cvtMat(fis[si._end]._m256, m_sum);
#else
	sis[i]._m256_mean = fis[(sis[i]._start + sis[i]._end) / 2]._m256.clone();
#endif
	//mat2fp(si._m256_mean, si._mhash);
}
