#include "CalcHistogram.h"

void Histogram1D::mat2GrayHist(Mat &image, int grayHist[64])
{
	Mat image_g;
	cvtColor(image, image_g, CV_BGR2GRAY);
	int sum = 0;
	Mat hist = getHistogram(image);
	for (int g = 0; g < 64; g++)
	{
		grayHist[g] = hist.at<float>(g);
		sum += grayHist[g];
	}
	//cout << "sum : " << sum << endl;
}

Mat CalcHistogram::getHistogram(const Mat &image)
{
	Mat hist;
	calcHist(&image,
		1,
		channels,
		Mat(),
		hist,
		dims,
		histSize,
		ranges,
		true,      //直方图每一维的histSize是均匀的
		false
		);

	return hist;
}

void CalcHistogram::mat2RGBHist(Mat &image, int hsvHist[64])
{
	//cvtColor(image, image, CV_BGR2HSV);
	Mat hist = getHistogram(image);
	int sum = 0;
	for (int v = 0; v < 4; v++)
	{
		for (int s = 0; s < 4; s++)
		{
			for (int h = 0; h < 4; h++)
			{
				hsvHist[v * 16 + s * 4 + h] = hist.at<float>(h, s, v);
			}
		}
	}
	
}

void CalcHistogram::getHistogramImage(const Mat &image)
{
	Mat hist = getHistogram(image);
	int scale = 4;
	int hbins = histSize[0];
	int sbins = histSize[1];
	int vbins = histSize[2];
	float *hist_sta = new float[sbins];
	float *hist_val = new float[vbins];
	float *hist_hue = new float[hbins];
	memset(hist_val, 0, vbins * sizeof(float));
	memset(hist_sta, 0, sbins * sizeof(float));
	memset(hist_hue, 0, hbins * sizeof(float));

	for (int s = 0; s < sbins; s++)
	{
		for (int v = 0; v < vbins; v++)
		{
			for (int h = 0; h<hbins; h++)
			{
				float binVal = hist.at<float>(h, s, v);
				cout << binVal << " ";
				hist_hue[h] += binVal;
				hist_val[v] += binVal;
				hist_sta[s] += binVal;
			}
		}
	}
#if 0
	for (int s = 0; s < sbins; s++)
		cout << hist_sta[s] << " ";
	for (int v = 0; v < vbins; v++)
		cout << hist_val[v] << " ";
	for (int h = 0; h < hbins; h++)
		cout << hist_hue[h] << " ";
#endif

	double max_sta = 0, max_val = 0, max_hue = 0;
	for (int i = 0; i<sbins; ++i)
	{
		if (hist_sta[i]>max_sta)
			max_sta = hist_sta[i];
	}
	for (int i = 0; i<vbins; ++i)
	{
		if (hist_val[i]>max_val)
			max_val = hist_val[i];
	}
	for (int i = 0; i<hbins; ++i)
	{
		if (hist_hue[i]>max_hue)
			max_hue = hist_hue[i];
	}

	Mat sta_img = Mat::zeros(240, sbins*scale + 20, CV_8UC3);
	Mat val_img = Mat::zeros(240, vbins*scale + 20, CV_8UC3);
	Mat hue_img = Mat::zeros(240, hbins*scale + 20, CV_8UC3);

	for (int i = 0; i<sbins; ++i)
	{
		int intensity = cvRound(hist_sta[i] * (sta_img.rows - 10) / max_sta);
		rectangle(sta_img, Point(i*scale + 10, sta_img.rows - intensity), Point((i + 1)*scale - 1 + 10, sta_img.rows - 1), Scalar(0, 255, 0), 1);
	}
	for (int i = 0; i<vbins; ++i)
	{
		int intensity = cvRound(hist_val[i] * (val_img.rows - 10) / max_val);
		rectangle(val_img, Point(i*scale + 10, val_img.rows - intensity), Point((i + 1)*scale - 1 + 10, val_img.rows - 1), Scalar(0, 0, 255), 1);
	}
	for (int i = 0; i<hbins; ++i)
	{
		int intensity = cvRound(hist_hue[i] * (hue_img.rows - 10) / max_hue);
		rectangle(hue_img, Point(i*scale + 10, hue_img.rows - intensity), Point((i + 1)*scale - 1 + 10, hue_img.rows - 1), Scalar(255, 0, 0), 1);
	}

	imshow("Shist", sta_img);
	imshow("Vhist", val_img);
	imshow("Hhist", hue_img);

	delete[] hist_sta;
	delete[] hist_val;
	delete[] hist_hue;
}


float calcCosSimi(int h0[64], int h1[64])
{
	int64 ss0 = 0, ss1 = 0;
	int64 hh = 0;
	for (int i = 0; i < 64; i++)
	{
		ss0 += h0[i] * h0[i];
		ss1 += h1[i] * h1[i];
		hh += h0[i] * h1[i];
	}
	int64 s0 = sqrt(ss0);
	int64 s1 = sqrt(ss1);
		
	//cout << float(hh) / (s0*s1) << endl;
	return float(hh) / (s0*s1);
}
