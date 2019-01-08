#ifndef CalcHistogram_H
#define CalcHistogram_H

#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;


class Histogram1D
{
private:
	int histSize[1]; // 项的数量
	float hranges[2]; // 统计像素的最大值和最小值
	const float* ranges[1];
	int channels[1]; // 仅计算一个通道

public:
	Histogram1D()
	{
		// 准备1D直方图的参数
		histSize[0] = 64;
		hranges[0] = 0.0f;
		hranges[1] = 256.0f;
		ranges[0] = hranges;
		channels[0] = 0;
	}

	MatND getHistogram(const Mat &image)
	{
		MatND hist;
		// 计算直方图
		calcHist(&image,// 要计算图像的
			1,                // 只计算一幅图像的直方图
			channels,        // 通道数量
			Mat(),            // 不使用掩码
			hist,            // 存放直方图
			1,                // 1D直方图
			histSize,        // 统计的灰度的个数
			ranges);        // 灰度值的范围
		return hist;
	}

	Mat getHistogramImage(const Mat &image)
	{
		MatND hist = getHistogram(image);

		// 最大值，最小值
		double maxVal = 0.0f;
		double minVal = 0.0f;

		minMaxLoc(hist, &minVal, &maxVal);

		//显示直方图的图像
		Mat histImg(histSize[0], histSize[0], CV_8U, Scalar(255));

		// 设置最高点为nbins的90%
		int hpt = static_cast<int>(0.9 * histSize[0]);
		//每个条目绘制一条垂直线
		for (int h = 0; h < histSize[0]; h++)
		{
			float binVal = hist.at<float>(h);
			int intensity = static_cast<int>(binVal * hpt / maxVal);
			// 两点之间绘制一条直线
			line(histImg, Point(h, histSize[0]), Point(h, histSize[0] - intensity), Scalar::all(0));
		}
		return histImg;
	}

	void mat2GrayHist(Mat &image, int grayHist[64]);

};


class CalcHistogram
{
private:
	int histSize[3];         //直方图项的数量
	float hranges[2];        //h通道像素的最小和最大值
	float sranges[2];
	float vranges[2];
	const float *ranges[3];  //各通道的范围
	int channels[3];         //三个通道
	int dims;

public:
	CalcHistogram(int hbins = 4, int sbins = 4, int vbins = 4)
	{
		histSize[0] = hbins;
		histSize[1] = sbins;
		histSize[2] = vbins;
		hranges[0] = 0; hranges[1] = 256;
		sranges[0] = 0; sranges[1] = 256;
		vranges[0] = 0; vranges[1] = 256;
		ranges[0] = hranges;
		ranges[1] = sranges;
		ranges[2] = vranges;
		channels[0] = 0;
		channels[1] = 1;
		channels[2] = 2;
		dims = 3;
	}

	Mat getHistogram(const Mat &image);
	void getHistogramImage(const Mat &image);
	void mat2RGBHist(Mat &image, int hsvHist[64]);

};

float calcCosSimi(int h0[64], int h1[64]);

#endif // CalcHistogram_H
