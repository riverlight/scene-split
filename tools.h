#ifndef TOOLS_H
#define TOOLS_H
#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include<algorithm>

using namespace cv;
using namespace std;

void Mat_2_structfp(Mat &m, int structfp[48 * 48]);
void mat2vfp(Mat s0, float m, int *vfp);
void mat2fp(Mat img, int *fp);
int calcScore(int *v0, int *v1);
void addMat(Mat &m_sum, Mat &m);
void cvtMat(Mat &m_8u3c, Mat &m_32f3c);
void int2string(string &str, int n);
int calcSAD(Mat m0, Mat m1);
void preprocessMat(Mat &src, Mat &dst);
void preprocessMat2(Mat &src, Mat &dst);

void getThumbnailBaseName(string &basename, char *vfile, char *tb_path);


#endif // TOOLS_H
