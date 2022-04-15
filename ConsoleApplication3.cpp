// ConsoleApplication3.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp>
#include "bp神经网络.h"
#include "opencv摄像头.h"
#include <windows.h>
using namespace cv;
using namespace std;
int ReverseInt(int i)
{
	unsigned char ch1, ch2, ch3, ch4;
	ch1 = i & 255;
	ch2 = (i >> 8) & 255;
	ch3 = (i >> 16) & 255;
	ch4 = (i >> 24) & 255;
	return((int)ch1 << 24) + ((int)ch2 << 16) + ((int)ch3 << 8) + ch4;
}

void test()
{
	unsigned char  labels = 0;
	unsigned char images[784] = { 0 };
	double nonumber[784] = { 0 };
	double exnumber[10] = { 0 };
	ifstream inimages("t10k-images.idx3-ubyte", ios::in | ios::binary); //图像数据
	ifstream inlabels("t10k-labels.idx1-ubyte", ios::in | ios::binary); //图像标签
	Neur_network a;
	a.read_data("wo.dat");
	int i = 0, j = 0, k = 10000, l = 0;
	inimages.seekg(16, ios::beg);
	inlabels.seekg(8, ios::beg);
	k = 10000;
	while (k--)
	{
		inimages.read((char*)&images, 784);
		for (i = 0; i < 784; i++)
			nonumber[i] = (images[i] - 127.5) / 1275.0;
		for (i = 0; i < 10; i++)
			exnumber[i] = 0;
		inlabels.read((char*)&labels, 1);
		exnumber[labels] = 1;
		a.input_layer_expected(nonumber, exnumber);
		a.forward_propagation_tanh();
		j = a.output();
		if (j == labels) l++;
		printf("识别数字为-%d-%d-为实际数字\n", j, labels);
	}
	printf("预测正确数：%d   \n", l);
	l = 0;
	inimages.close();
	inlabels.close();
}/**/
void train()
{
	unsigned char  labels = 0;
	unsigned char images[784] = { 0 };
	double nonumber[784] = { 0 };
	double exnumber[10] = { 0 };
	ifstream inimages("train-images.idx3-ubyte", ios::in | ios::binary); //图像数据
	ifstream inlabels("train-labels.idx1-ubyte", ios::in | ios::binary); //图像标签
	int s[] = { 784,16,16,10 };
	int o = sizeof(s) / sizeof(int);
	Neur_network a;
	a.train_data(o, 0.01, s);
	a.initializing_even();
	int i = 0, j = 1, k = 0;
	double l = 0;
	while (j)
	{
		inimages.seekg(16, ios::beg);
		inlabels.seekg(8, ios::beg);
		l = 0;
		k = 60000;
		while (k--)
		{
			inimages.read((char*)&images, 784);
			for (i = 0; i < 784; i++)
				nonumber[i] = (images[i] - 127.5) / 1275.0;
			for (i = 0; i < 10; i++)
				exnumber[i] = 0;
			inlabels.read((char*)&labels, 1);
			exnumber[labels] = 1;
			a.input_layer_expected(nonumber, exnumber);
			a.forward_propagation_tanh();
			a.back_propagation_tanh();
			l += a.loss();
		}
		printf("损失函数 = %.10f   \n", l / 60000.0);
		printf("输入0结束训练,其他继续训练。\n");
		cin >> j;		
	}
	a.write_data("wo.dat");
}
/*
void aa()
{
	double nonumber[784] = { 0 };
	double exnumber[10] = { 0 };
	Mat m = imread("60.jpg");
	Mat m1(28, 28, CV_8U);
	cvtColor(m, m1, COLOR_RGB2GRAY);
	for (int i = 0; i < 784; i++)
		nonumber[i] = (m1.data[i] - 127.5) / 1275.0;
	Neur_network g("9387.dat");
	g.input_layer_expected(nonumber, exnumber);
	g.forward_propagation_tanh();
	g.print();
	printf("正确数：%d   \n", g.output());
}
void caa(uchar* m1, uchar* m2)//m1复制到m2
{
	int i = 0;
	for (int j = 0; j < 280; j++)
	{
		for (int k = (100 + j) * 640 + 180; k < (100 + j) * 640 + 460; k++)
		{
			m2[i++] = m1[k];
		}
	}
}
void cc()
{
	Camera_bmp a;
	a.open_camera();
	a.collect_gray();
	Neur_network g("9387.dat");
	double exnumber[10] = { 0 };
	g.input_layer_expected(a.nonumber, exnumber);
	g.forward_propagation_tanh();
	printf("----------------\n");
	g.output();
	printf("----------------\n");
	//g.print();
}
void sav()
{
	unsigned char  labels = 0;
	String s;
	String s1("60.jpg");
	Mat shrink(28, 28, CV_8U);
	ifstream inimages("t10k-images.idx3-ubyte", ios::in | ios::binary); //图像数据
	ifstream inlabels("t10k-labels.idx1-ubyte", ios::in | ios::binary); //图像标签
	inimages.seekg(16, ios::beg);
	inlabels.seekg(8, ios::beg);
	for (int i = 0; i < 10; i++)
	{
		inimages.read((char*)shrink.data, 784);
		inlabels.read((char*)&labels, 1);
		//if (labels == 5)break;

	}
	for (int i = 0; i < 100; i++)
	{
		inimages.read((char*)shrink.data, 784);
		inlabels.read((char*)&labels, 1);
		if (labels == 6)break;
		
	}
	imwrite(s + s1, shrink);	
	inimages.close();
	inlabels.close();
}
/**/
int main()
{
	//train();
	test();
	return 0;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
