#include "ImageWithProjection.h"
#include "Common.h"

static int patchWidth = 256;
static int patchHeight = 256;

/*
create image with projection
*/
ImageWithProjection::ImageWithProjection() {
	
}

ImageWithProjection::~ImageWithProjection() {

}

bool ImageWithProjection::is_proj(QString path)
{
	//judge that whether image is remote sensing image
	GDALDataset* tempDataset = (GDALDataset*)GDALOpen(path.toStdString().c_str(), GA_ReadOnly);
	QString prj = tempDataset->GetProjectionRef();
	if (tempDataset != NULL) GDALClose(tempDataset);
	return prj == NULL ? false : true;
}

Mat ImageWithProjection::readImageWithProj(const QString path, int* selectBand) {
	GDALDataset *gdalDataset = (GDALDataset*)GDALOpen(path.toStdString().c_str(), GA_ReadOnly);

	//Height, Width and BandNum of image
	int imgHeight = gdalDataset->GetRasterYSize();
	int imgWidth = gdalDataset->GetRasterXSize();
	int imgBandNum = gdalDataset->GetRasterCount();

	//µ¥²¨¶ÎÍ¼Ïñ´¦Àí
	if (imgBandNum == 1) { selectBand[1] = 1; selectBand[2] = 1;}

	//block size
	int blockWidth = patchWidth;
	int blockHeight = patchHeight;
	//set height, width of block to block processing
	int blockXNum = (imgWidth - 1) / blockWidth + 1;
	int blockYNum = (imgHeight - 1) / blockHeight + 1;

	GDALDataType tempType = gdalDataset->GetRasterBand(1)->GetRasterDataType();

	Mat mergeMatWidth;
	Mat mergeMatHeight;
	std::vector<cv::Mat> imgMat(3);
	
	for (int i = 0; i < 3; ++i)
	{
		GDALRasterBand *tempBand = gdalDataset->GetRasterBand(selectBand[i]);
		//patch-read
		for (int iy = 0; iy < blockYNum; ++iy) {
			for (int ix = 0; ix < blockXNum; ++ix) {
				//default static int is 512
				blockHeight = patchHeight;
				blockWidth = patchWidth;
				if (iy == blockYNum - 1) {
					blockHeight = imgHeight % blockHeight;
				}
				if (ix == blockXNum - 1) {
					blockWidth = imgWidth % blockWidth;
				}
				unsigned char *bufferByte = new unsigned char[blockWidth * blockHeight];
				float *bufferFloat = new float[blockWidth * blockHeight];

				//if data type is byte, read it straightly
				if (tempType == GDT_Byte) 
				{					
					tempBand->RasterIO(GF_Read, ix * patchWidth, iy * patchHeight, blockWidth, blockHeight, bufferByte, blockWidth, blockHeight, GDT_Byte, 0, 0);
				}
				else 
				{				
					tempBand->RasterIO(GF_Read, ix * patchWidth, iy * patchHeight, blockWidth, blockHeight, bufferFloat, blockWidth, blockHeight, GDT_Float32, 0, 0);
					bufferByte = imgSketchFloat2Byte(bufferFloat, tempBand, blockWidth * blockHeight, tempBand->GetNoDataValue());
					//if (bufferFloat) { delete[] bufferFloat; bufferFloat = NULL; }
				}
				cv::Mat tempMat(blockHeight, blockWidth, 0, bufferByte);
				//if (bufferByte) { delete[] bufferByte; bufferByte = NULL; }
				if (ix > 0){
					mergeMatWidth = Common::mergeWidth(mergeMatWidth, tempMat);
					continue;
				}
				//uchar pixelValue = tempMat.at<uchar>(100, 100);
				mergeMatWidth = tempMat.clone();
			}
			if (iy > 0){
				mergeMatHeight = Common::mergeHeight(mergeMatHeight, mergeMatWidth);
				continue;
			}
			mergeMatHeight = mergeMatWidth.clone();
		}
		imgMat.at(2 - i) = mergeMatHeight.clone();
		mergeMatWidth = NULL;
		mergeMatHeight = NULL;
	}
	//it should be cv::Mat *. if it is cv::Mat, it can not display(the whole white)
	cv::Mat* mergeImgMat = new cv::Mat;
	cv::merge(imgMat, *mergeImgMat);
	imgMat.clear();
	if (gdalDataset) GDALClose(gdalDataset);
	return *mergeImgMat;
}

unsigned char *ImageWithProjection::imgSketchFloat2Byte(float *imgFloat, GDALRasterBand *band, int size, float noValue) {
	unsigned char *bufferByte = new unsigned char[size];
	memset(bufferByte, 0, size);
	float max, min;
	double minmax[2];
    //precise search's minimium is -32768 when parameter is false
	band->ComputeRasterMinMax(true, minmax);
	min = minmax[0];
	max = minmax[1];
	if (min <= noValue && noValue <= max){
		min = 0.0;
	}
	for (int i = 0; i < size; i++){
		if (imgFloat[i] > max){
			bufferByte[i] = 255;
		}
		else if (imgFloat[i] <= max && imgFloat[i] >= min){
			bufferByte[i] = static_cast<unsigned char>(round((imgFloat[i] - min) / (max - min)  * 255.0));
		}
		else{
			bufferByte[i] = 0;
		}
	}
	return bufferByte;
}