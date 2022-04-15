#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp>
#define WIDTH 280
#define HEIGHT 280
class Camera_bmp
{
public:
	Camera_bmp();
	~Camera_bmp();
	void open_camera();//��������ͷ
	void collect_gray();//�ɼ����Ҷ�ͼ
	double nonumber[784];//��������������
	int width;
	int height;
	cv::Mat camera_filp;//����ͷ��ת��
private:
	cv::Mat camera_bmp;//����ͷԭͼ
	cv::VideoCapture camera;//����ͷ
	cv::Mat gray;//�Ҷ�ͼ
	cv::Mat collect;//�ɼ����Ҷ�ͼ
	cv::Rect reg;//�ɼ�����Χ
	cv::Scalar color;
	cv::Mat median;//
	cv::Mat shrink;//�ɼ�����С��Ҷ�ͼ
};
Camera_bmp::Camera_bmp() :camera(0), camera_bmp(), collect(HEIGHT, WIDTH, CV_8U),
color(0, 255, 0), shrink(28, 28, CV_8U), median(), gray()
{
	memset(nonumber, 0, 784 * sizeof(double));
	width = (int)camera.get(3);
	height = (int)camera.get(4);
	reg.x = width / 2 - WIDTH / 2;
	reg.y = height / 2 - HEIGHT / 2;
	reg.width = WIDTH;
	reg.height = HEIGHT;
}
Camera_bmp::~Camera_bmp()
{
	camera.release();//�ͷ�����ͷ
}
void Camera_bmp::open_camera()
{
	camera >> camera_bmp;
	cv::flip(camera_bmp, camera_filp, -1);
	cv::rectangle(camera_filp, reg, color, 1, cv::LINE_AA);
}
void Camera_bmp::collect_gray()
{
	cv::medianBlur(camera_bmp, median, 3);//��ֵ�˲�
	cv::cvtColor(median, gray, cv::COLOR_RGB2GRAY);
	//cv::cvtColor(camera_bmp, gray, cv::COLOR_RGB2GRAY);
	int i = 0;
	for (int j = 0; j < HEIGHT; j++)
	{
		for (int k = (reg.y + j) * width + reg.x; k < (reg.y + j) * width + reg.x + WIDTH; k++)
		{
			collect.data[i++] = gray.data[k];
		}
	}
	cv::resize(collect, shrink, cv::Size(28, 28), 0, 0);
	for (int i = 0; i < 784; i++)
		nonumber[i] = (shrink.data[i] - 127.5) / 1275.0;
}