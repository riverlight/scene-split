#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include<algorithm>
#include "tools.h"

using namespace cv;
using namespace std;


void Mat_2_structfp(Mat &m, int structfp[48 * 48])
{
	Mat m_48;
	if (m.cols != 48 || m.rows != 48)
		resize(m, m_48, Size(48, 48));
	else
		m_48 = m.clone();
	cvtColor(m_48, m_48, CV_BGR2GRAY);

	Mat imgThres;
	threshold(m_48, imgThres, 255, 1, THRESH_OTSU);

	for (int i = 0; i < 48; i++)
		for (int j = 0; j < 48; j++)
			structfp[i * 48 + j] = imgThres.at<uchar>(i, j);
	//imshow("88", imgThres);
	//waitKey();
}


void mat2vfp(Mat s0, float m, int *vfp)
{
	int i, j;
	uchar *p0;
	for (i = 0; i < s0.rows; i++)
	{
		p0 = s0.ptr<uchar>(i);
		for (j = 0; j < s0.cols; j++)
		{
			int v0, v1;
			v0 = p0[j];
			if (float(v0)>m)
				vfp[i * 8 + j] = 1;
			else
				vfp[i * 8 + j] = 0;
		}
	}
}

void mat2fp(Mat img, int *fp)
{
	Mat s0;
	cvtColor(img, s0, COLOR_BGR2GRAY);
	Rect r0 = { 0, 0, 60, 30 };
	Rect r1 = { 195, 0, 60, 30 };
	Rect r2 = { 0, 225, 60, 30 };
	Rect r3 = { 195, 225, 60, 30 };

	Mat roi0 = s0(r0);
	Mat roi1 = s0(r1);
	Mat roi2 = s0(r2);
	Mat roi3 = s0(r3);
	roi0 = 128;
	roi1 = 128;
	roi2 = 128;
	roi3 = 128;
	//imshow("99", s0);
	//waitKey(0);
	//exit(0);

	//equalizeHist(s0, s0);
	Size s = { 8, 8 };

	Mat img8;
	resize(s0, img8, s);
	Scalar m = mean(img8);
	mat2vfp(img8, m[0], fp);
}

int calcScore(int *v0, int *v1)
{
	int s = 0;
	for (int i = 0; i < 64; i++)
		if (v0[i] == v1[i])
			s++;

	return s;
}

int calcSAD(Mat m0, Mat m1)
{
	Mat m_ad;
	Scalar m;
	absdiff(m0, m1, m_ad);
	m = sum(m_ad);

	return 256.0 - float(m[0] + m_ad.cols*m_ad.rows/2) / (m_ad.cols*m_ad.rows);
}


void addMat(Mat &m_sum, Mat &m)
{
	int i, j;
	float *p0;
	uchar *p1;
	for (i = 0; i < m_sum.rows; i++)
	{
		p0 = m_sum.ptr<float>(i);
		p1 = m.ptr<uchar>(i);
		for (j = 0; j < m_sum.cols; j++)
		{
			p0[3 * j + 0] += p1[3 * j + 0];
			p0[3 * j + 1] += p1[3 * j + 1];
			p0[3 * j + 2] += p1[3 * j + 2];
		}
	}
}

void cvtMat(Mat &m_8u3c, Mat &m_32f3c)
{
	m_8u3c = Mat::zeros(256, 256, CV_8UC3);

	int i, j;
	uchar *p0;
	float *p1;
	for (i = 0; i < m_8u3c.rows; i++)
	{
		p0 = m_8u3c.ptr<uchar>(i);
		p1 = m_32f3c.ptr<float>(i);
		for (j = 0; j < m_8u3c.cols; j++)
		{
			p0[3 * j + 0] = p1[3 * j + 0] + 0.5;
			p0[3 * j + 1] = p1[3 * j + 1] + 0.5;
			p0[3 * j + 2] = p1[3 * j + 2] + 0.5;
		}
	}
}

void int2string(string &str, int n)
{
	stringstream stream;
	stream << n;
	stream >> str;
}

void preprocessMat2(Mat &src, Mat &dst)
{
	Size s256;
	if (src.rows>src.cols)
	{
		s256.width = 128;
		s256.height = 128 * float(src.rows) / float(src.cols);
	}
	else
	{
		s256.height = 128;
		s256.width = 128 * float(src.cols) / float(src.rows);
	}
	resize(src, dst, s256);
}

void preprocessMat(Mat &src, Mat &dst)
{
	Mat imgTmp, imgTmp_g;
	Size s256, osize = { 256, 256 }, glcm_size;
	if (src.rows == src.cols)
	{
		resize(src, dst, osize);
	} else
	if (src.rows>src.cols)
	{
		s256.width = 256;
		s256.height = 256 * float(src.rows) / float(src.cols);

		resize(src, imgTmp, s256);
		cvtColor(imgTmp, imgTmp_g, CV_BGR2GRAY);
		int i;
		for (i = 128; i < s256.height / 2 - 10; i++)
		{
			Mat roi[2];
			Rect r[2];
			r[0] = { 0, i, 256, 1 };
			roi[0] = imgTmp_g(r[0]);
			r[1] = { 0, s256.height - i, 256, 1 };
			roi[1] = imgTmp_g(r[1]);
			if (sum(roi[0])[0] > 256 * 17 || sum(roi[1])[0] > 256 * 17)
				break;
		}
		if (i == 128)
		{
			// 计算左右的黑条
			int count_l, count_r;
			for (count_l = 0; count_l < src.cols / 4; count_l++)
			{
				Rect r(count_l, 0, 1, src.rows);
				if (sum(src(r))[0] > src.rows * 17)
					break;
			}
			for (count_r = src.cols - 1; count_r > (src.cols * 3) / 4; count_r--)
			{
				Rect r(count_r, 0, 1, src.rows);
				//cout << "sum : " << sum(src(r)) << endl;
				if (sum(src(r))[0] > src.rows * 17)
					break;
			}
			Rect src_r(count_l, 0, count_r - count_l + 1, src.rows);
			//cout << src_r << " " << src.cols << " " << count_r<< endl;
			resize(src(src_r), dst, osize);

		}
		else
		{
			Rect r = { 0, i, 256, s256.height - 2 * i };
			Mat roi = imgTmp(r);
			resize(roi, dst, osize);

		}
	}
	else
	{
		s256.height = 256;
		s256.width = 256 * float(src.cols) / float(src.rows);

		resize(src, imgTmp, s256);
		cvtColor(imgTmp, imgTmp_g, CV_BGR2GRAY);
		int i;
		for (i = 128; i < s256.width/2 - 10; i++)
		{
			Mat roi[2];
			Rect r[2];
			r[0] = { i, 0, 1, 256 };
			roi[0] = imgTmp_g(r[0]);
			r[1] = { s256.width - i, 0, 1, 256 };
			roi[1] = imgTmp_g(r[1]);
			if (sum(roi[0])[0] > 256 * 17 || sum(roi[1])[0] > 256 * 17)
				break;
		}
		if (i == 128)
		{
			// 计算上下的黑条
			int count_t, count_b;
			for (count_t = 0; count_t < src.rows / 4; count_t++)
			{
				Rect r(0, count_t, src.cols, 1);
				if (sum(src(r))[0] > src.cols * 17)
					break;
			}
			for (count_b = src.rows-1; count_b > (src.rows*3) / 4; count_b--)
			{
				Rect r(0, count_b, src.cols, 1);
				if (sum(src(r))[0] > src.cols * 17)
					break;
			}
			Rect src_r(0, count_t, src.cols, count_b-count_t+1);
			resize(src(src_r), dst, osize);

		}
		else
		{
			Rect r = { i, 0, s256.width - 2 * i, 256 };
			Mat roi = imgTmp(r);
			resize(roi, dst, osize);

		}
	}
}

//注意：当字符串为空时，也会返回一个空字符串  
static void split(std::string& s, std::string& delim, std::vector< std::string > &ret)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		ret.push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last>0)
	{
		ret.push_back(s.substr(last, index - last));
	}
}

void getThumbnailBaseName(string &basename, char *vfile, char *tb_path)
{
	std::string sz_vfile = vfile;
	std::string sz_tb_path = tb_path;

	vector<string> v_vfile_dot;
	string sz_dot = ".";
	split(sz_vfile, sz_dot, v_vfile_dot);
	//cout << v_vfile_dot[0] << endl;

	vector<string> v_vfile_yy;
	string sz_yy = "/";
	split(v_vfile_dot[0], sz_yy, v_vfile_yy);
	//cout << v_vfile_yy[v_vfile_yy.size() - 1] << endl;

	vector<string> v_vfile_xx;
	string sz_xx = "\\";
	split(v_vfile_yy[v_vfile_yy.size() - 1], sz_xx, v_vfile_xx);
	//cout << v_vfile_xx[v_vfile_xx.size() -1] << endl;

	basename = sz_tb_path + "\\" + v_vfile_xx[v_vfile_xx.size() - 1];
	//cout << "basename " << basename << endl;
}
