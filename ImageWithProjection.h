#pragma once

#ifndef ImageWithProjection_H
#define ImageWithProjection_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <gdal_priv.h>
#include <vector>
#include <QString>
#include <iostream>

#pragma comment(lib,"gdal_i.lib")

namespace cv
{
	using std::vector;
}
using
namespace  cv;

using
namespace  std;

class ImageWithProjection {

public :
	ImageWithProjection();
	~ImageWithProjection();

	static bool is_proj(QString path);
	static Mat readImageWithProj(const QString path, int* selectBand);
	static unsigned char* imgSketchFloat2Byte(float* imgFloat, GDALRasterBand* band, int size, float noValue);
};

#endif