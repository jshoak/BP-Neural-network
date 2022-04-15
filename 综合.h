#pragma once
#include "bpÉñ¾­ÍøÂç.h"
#include "opencvÉãÏñÍ·.h"
#include <thread>
#include <mutex>
#include <windows.h>
#define BYTES3(i) (i * 3)
#define BYTES4(i) (4 - (BYTES3(i) % 4))
#define WIDTHBYTES(i) (BYTES3(i) % 4 ? BYTES3(i) + BYTES4(i) : i * 3)
class Colligate
{
public:
	Colligate();
	~Colligate();
	void hwndshow(HWND hWnd);//ÏÔÊ¾Í¼Ïñ
	int ledntify();//Ê¶±ðÍ¼Ïñ
private:
	void show();
	Camera_bmp bmp;
	Neur_network l;
	BITMAPINFOHEADER bih;
	HWND DeskWnd;
	HDC memDC;//¼æÈÝDC
	HDC DeskDC;//´°¿ÚDC
	HBITMAP DeskBmp;//¼æÈÝÎ»Í¼ 
	mutex mut;//»¥³âËø
	std::thread t;
	bool switc;
};

Colligate::Colligate() :bmp(), t(), switc(1), mut(), l()
{
	memset(&bih, 0, sizeof(BITMAPINFOHEADER));
	l.read_data("123.dat");
}

Colligate::~Colligate()
{
	switc = 0;
	t.join();
}

void Colligate::show()
{
	while (switc)
	{
		mut.lock();
		bmp.open_camera();
		mut.unlock();
		SetDIBits(memDC, DeskBmp, 0, bih.biHeight, bmp.camera_filp.data, (BITMAPINFO*)&bih, DIB_RGB_COLORS);
		StretchBlt(DeskDC, 0, 0, bih.biWidth, bih.biHeight, memDC, 0, 0, bih.biWidth, bih.biHeight, SRCCOPY);
	}
}

void Colligate::hwndshow(HWND hWnd)
{
	bih.biSize = 40;
	bih.biWidth = bmp.width;
	bih.biHeight = bmp.height;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = bih.biHeight * WIDTHBYTES(bih.biWidth);
	DeskDC = GetDC(hWnd);;//´°¿ÚDC
	memDC = CreateCompatibleDC(DeskDC);//¼æÈÝDC
	DeskBmp = CreateCompatibleBitmap(DeskDC, bih.biWidth, bih.biHeight);;//¼æÈÝÎ»Í¼
	SelectObject(memDC, DeskBmp);
	t = std::thread(&Colligate::show, this);
}

int Colligate::ledntify()
{
	double exnumber[10] = { 0 };
	mut.lock();
	bmp.collect_gray();
	mut.unlock();
	l.input_layer_expected(bmp.nonumber, exnumber);
	l.forward_propagation_tanh();
	int number = l.output();
	return number;
}


