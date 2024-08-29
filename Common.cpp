#include "Common.h"
#include <iostream>

QStringList Common::findImageFilesInDirectory(const QString& directoryPath) 
{
	QStringList tmpImageFiles;
	QDir directory(directoryPath);
	QStringList filters = { "*.png", "*.jpg", "*.jpeg", "*.gif", "*.bmp" , "*.tif", "*.tiff" };
	QList<QFileInfo> fileInfoList = directory.entryInfoList(filters, QDir::Files);

	for (const QFileInfo& fileInfo : fileInfoList)
	{
		tmpImageFiles.append(fileInfo.filePath());
	}
	return tmpImageFiles;
}

QStringList Common::findVectorFilesInDirectory(const QString& directoryPath) 
{
	QStringList tmpVectorFiles;
	QDir directory(directoryPath);
	QStringList filters = { "*.shp" };
	QList<QFileInfo> fileInfoList = directory.entryInfoList(filters, QDir::Files);

	for (const QFileInfo& fileInfo : fileInfoList)
	{
		tmpVectorFiles.append(fileInfo.filePath());
	}
	return tmpVectorFiles;
}

bool Common::isImageFile(const QString& filePath)
{
	// 添加你认为是图像文件的检测逻辑，例如通过文件扩展名或者文件内容检查
	// 在这个简单的示例中，我们只检查了文件扩展名是否为图片格式
	QStringList imageExtensions = { "png", "jpg", "jpeg", "gif", "bmp" , "tif", "tiff" };
	QString extension = QFileInfo(filePath).suffix().toLower();
	return imageExtensions.contains(extension);
}

QString Common::openDirectory(QWidget* parent, const QString& caption)
{
	QString directoryPath = QFileDialog::getExistingDirectory(
		parent,
		caption,
		"/home",
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	);

	if (directoryPath.isEmpty()) {
		QMessageBox::about(NULL, "Error", "Directory is empty");
		return QString();  // 返回一个空字符串表示错误
	}
	return directoryPath;
}

QStringList Common::openFile(QWidget* parent, const QString& caption, const QString& type)
{
	QStringList filePaths = QFileDialog::getOpenFileNames(parent,
		caption,
		"/home",
		type);
	if (filePaths.isEmpty()) {
		QMessageBox::about(NULL, "Error", "Files is empty");
		return QStringList();
	}
	return filePaths;
}

QString Common::saveImageFile()
{
	QString filePath = QFileDialog::getSaveFileName(nullptr, "Save image", "", "Images (*.png *.jpg *.jpeg *.bmp *.tif)");
	return filePath;
}

/***************************************
              For scene
***************************************/
bool Common::isPointInRect(QPointF point, QRectF rect)
{
	if (rect.top() > point.y() || (rect.top() + rect.height()) < point.y() || \
		rect.left() > point.x() || (rect.left() + rect.width()) < point.x())
		return false;
	return true;
}

bool Common::isPointInRectFromCenter(QPointF point, QPointF center, int width, int height)
{
	QPointF lt = getLTPointFromCenter(center, width, height);
	QPointF rb = lt + QPointF(width, height);
	if (point.x() >= lt.x() && point.x() <= rb.x() && point.y() >= lt.y() && point.y() <= rb.y())
		return true;
	return false;
}

cv::Mat Common::getImagePatch(QPixmap pixmap, QPointF center, int sliceWidth, int sliceHeight)
{
	cv::Mat mat = Common::pixmapToMat(pixmap);
	cv::Mat slice = Common::getSliceAroundCenter(mat, center, sliceWidth, sliceHeight);
	return slice;
}

cv::Mat Common::getImageScale(cv::Mat mat, int targetWidth, int targetHeight)
{
	cv::Mat slice;
	cv::resize(mat, slice, cv::Size(targetWidth, targetHeight));
	return slice;
}

cv::Mat Common::getSliceAroundCenter(const cv::Mat& originalImage, QPointF center, int sliceWidth, int sliceHeight)
{
	QPointF pointLT = getLTPointFromCenter(center, sliceWidth, sliceHeight);
	int sliceX = static_cast<int>(pointLT.x());
	int sliceY = static_cast<int>(pointLT.y());

	// 计算切片的右下角坐标
	int sliceWidthEnd = std::min(originalImage.cols - 1, sliceX + sliceWidth);
	int sliceHeightEnd = std::min(originalImage.rows - 1, sliceY + sliceHeight);

	// 裁剪原始图像以获取切片
	cv::Rect roi(sliceX, sliceY, sliceWidthEnd - sliceX, sliceHeightEnd - sliceY);
	cv::Mat slice = originalImage(roi).clone(); // 使用 clone() 创建切片的副本

	return slice;
}

cv::Mat Common::pixmapToMat(QPixmap pixmap)
{
	// 将图像转换为 QImage
	QImage image = pixmap.toImage();
	// 将 QImage 转换为 cv::Mat
	cv::Mat mat;
	if (image.format() == QImage::Format_RGB32 || image.format() == QImage::Format_ARGB32)
	{
		mat = cv::Mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine());
		cv::cvtColor(mat, mat, cv::COLOR_RGBA2BGR);
	}
	else
	{
		mat = cv::Mat(image.height(), image.width(), CV_8UC3, const_cast<uchar*>(image.bits()), image.bytesPerLine());
	}
	// 如果需要将 BGR 转换为 RGB，请使用以下行
	cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
	return mat;
}

QPixmap Common::matToPixmap(const cv::Mat& image)
{
	// 将 OpenCV 的 Mat 转换为 Qt 的 QImage
	QImage qImage(image.data, image.cols, image.rows, static_cast<int>(image.step), QImage::Format_RGB888);
	qImage = qImage.rgbSwapped(); // 因为 OpenCV 默认使用 BGR 格式，而 Qt 使用 RGB 格式，需要进行颜色通道交换
	// 将 QImage 转换为 QPixmap
	QPixmap pixmap = QPixmap::fromImage(qImage);
	// 创建 QGraphicsPixmapItem 并设置其 pixmap
	return pixmap;
}

QPolygonF Common::changeRectBeginEnd(QPolygonF polygon)
{
	QPointF begin = polygon[0];
	QPointF end = polygon[polygon.count() - 1];
	QPointF beginUpdate = begin.x() <= end.x() ? begin : end;
	QPointF endUpdate = begin.x() <= end.x() ? end : begin;
	qreal x = beginUpdate.x();
	qreal y = beginUpdate.y();
	qreal w = endUpdate.x() - x;
	qreal h = endUpdate.y() - y;
	QPolygonF tempPolygon;
	tempPolygon.append(QPointF(x, y));
	tempPolygon.append(QPointF(x + w, y));
	tempPolygon.append(QPointF(x + w, y + h));
	tempPolygon.append(QPointF(x, y + h));
	return tempPolygon;
}

QPointF Common::getLTPointFromCenter(QPointF center, int width, int height)
{
	int xA = static_cast<int>(center.x());
	int yA = static_cast<int>(center.y());
	// 计算选定框的左上角坐标
	int sliceX = std::max(0, xA - width / 2);
	int sliceY = std::max(0, yA - height / 2);
	return QPointF(sliceX, sliceY);
}

QPointF Common::getPointImageToPatchFromCenter(QPointF point, QPointF center, int width, int height)
{
	int xA = static_cast<int>(center.x());
	int yA = static_cast<int>(center.y());
	// 计算选定框的左上角坐标
	int sliceX = std::max(0, xA - width / 2);
	int sliceY = std::max(0, yA - height / 2);

	QPointF pointInPatch(point.x() - sliceX, point.y() - sliceY);
	return pointInPatch;
}

QPointF Common::getPointScale(QPointF point, int originalWidth, int originalHeight, int newWidth, int newHeight)
{
	float x = point.x() * newWidth / originalWidth;
	float y = point.y() * newHeight / originalHeight;
	QPointF pointScale(x, y);
	return pointScale;
}

QRectF Common::updateRectFromCenter(QPointF center, int width, int height)
{
	QPointF leftTop(center.x() - width / 2, center.y() - height / 2);
	QPointF rightBottom(center.x() + width / 2, center.y() + height / 2);
	return QRectF(leftTop, rightBottom);
}

bool Common::isRectExceed(QPointF center, QRectF rect, int width, int height)
{
	float left = center.x() - width / 2;
	float right = center.x() + width / 2;
	float top = center.y() - height / 2;
	float bottom = center.y() + height / 2;
	if (rect.top() > top || (rect.top() + rect.height()) < bottom || \
		rect.left() > left || (rect.left() + rect.width()) < right)
		return true;
	return false;
}

std::vector<std::vector<int>> Common::getClipBoxList(int width, int height, int clipWidth, int clipHeight, int overlap, bool direction) {
	int start_w = 0;
	int start_h = 0;
	int end_w = clipWidth;
	int end_h = clipHeight;
	std::vector<std::vector<int>> cropBoxList;

	if (!direction) {
		while (start_h < height) {
			if (end_h > height) {
				end_h = height;
			}
			while (start_w < width) {
				if (end_w > width) {
					end_w = width;
				}
				cropBoxList.push_back({ start_h, end_h, start_w, end_w });
				if (end_w == width) break;
				start_w = end_w - overlap;
				end_w = start_w + clipWidth;
			}
			if (end_h == height) break;
			start_h = end_h - overlap;
			end_h = start_h + clipHeight;
			start_w = 0;
			end_w = clipWidth;
		}
	}
	else {
		while (start_w < width) {
			if (end_w > width) {
				end_w = width;
			}
			while (start_h < height) {
				if (end_h > height) {
					end_h = height;
				}
				cropBoxList.push_back({ start_h, end_h, start_w, end_w });
				if (end_h == height) break;
				start_h = end_h - overlap;
				end_h = start_h + clipHeight;
			}
			if (end_w == width) break;
			start_w = end_w - overlap;
			end_w = start_w + clipWidth;
			start_h = 0;
			end_h = clipHeight;
		}
	}
	return cropBoxList;
}

//恢复大图的大图坐标
std::vector<std::vector<int>> Common::getRecoverList(std::vector<std::vector<int>> clipBoxList, int baseImageWidth, int baseImageHeight, int Width, int Height,  int overlap)
{
	int half_overlap = overlap / 2;
	std::vector<std::vector<int>> recoverList;
	for (int i = 0; i < clipBoxList.size(); i++)
	{
		int shift_x = clipBoxList[i][2], shift_y = clipBoxList[i][0]; //左上角坐标
		int shift_x_ = clipBoxList[i][3], shift_y_ = clipBoxList[i][1]; //右上角坐标
		int start_x, end_x, start_shift_x, end_shift_x;
		shift_x == 0 ? start_x = 0 : start_x = half_overlap;
		end_x = Width - half_overlap;
		start_shift_x = shift_x + start_x;
		end_shift_x = shift_x + end_x;
		if (shift_x + Width > baseImageWidth)
		{
			end_shift_x = baseImageWidth;
			end_x = shift_x_ - shift_x;
		}
		// y orientation
		int start_y, end_y, start_shift_y, end_shift_y;
		shift_y == 0 ? start_y = 0 : start_y = half_overlap;
		end_y = Height - half_overlap;
		start_shift_y = shift_y + start_y;
		end_shift_y = shift_y + end_y;
		if (shift_y + Height > baseImageHeight)
		{
			end_shift_y = baseImageHeight;
			end_y = shift_y_ - shift_y;
		}

		recoverList.push_back({ start_shift_x, end_shift_x, start_shift_y, end_shift_y, start_x, end_x, start_y, end_y }); //前四个为大图的对应坐标，后四个为小图的对应坐标
	}
	return recoverList;
}

/***************************************
	    basic image processing
***************************************/
cv::Mat Common::mergeHeight(cv::Mat A, cv::Mat B)
{
	int totalRows = A.rows + B.rows;
	cv::Mat mergedDescriptors(totalRows, A.cols, A.type());
	cv::Mat submat = mergedDescriptors.rowRange(0, A.rows);
	A.copyTo(submat);
	submat = mergedDescriptors.rowRange(A.rows, totalRows);
	B.copyTo(submat);
	return mergedDescriptors;
}

cv::Mat Common::mergeWidth(cv::Mat A, cv::Mat B)
{
	int totalCols = A.cols + B.cols;
	cv::Mat mergedDescriptors(A.rows, totalCols, A.type());
	cv::Mat submat = mergedDescriptors.colRange(0, A.cols);
	A.copyTo(submat);
	submat = mergedDescriptors.colRange(A.cols, totalCols);
	B.copyTo(submat);
	return mergedDescriptors;
}