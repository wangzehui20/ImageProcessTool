#include <QDir>
#include <QList>
#include <QFileDialog>
#include <QMessageBox>
#include <opencv2/opencv.hpp>

class Common 
{
public:
	static bool isImageFile(const QString& filePath);
	static QString openDirectory(QWidget* parent, const QString& caption);
	static QStringList findImageFilesInDirectory(const QString& directoryPath);
	static QStringList findVectorFilesInDirectory(const QString& directoryPath);
	static QStringList openFile(QWidget* parent, const QString& caption, const QString& type);
	static QString saveImageFile();
	
	// scene functions
	static bool isPointInRect(QPointF point, QRectF rect);
	static bool isPointInRectFromCenter(QPointF point, QPointF center, int width, int height);
	static bool isRectExceed(QPointF center, QRectF rect, int width, int height);
	static cv::Mat getImagePatch(QPixmap pixmap, QPointF center, int sliceWidth, int sliceHeight);
	static cv::Mat getImageScale(cv::Mat mat, int targetWidth, int targetHeight);
	static cv::Mat getSliceAroundCenter(const cv::Mat& originalImage, QPointF center, int sliceWidth, int sliceHeight);
	static cv::Mat pixmapToMat(QPixmap pixmap);
	static QPixmap matToPixmap(const cv::Mat& image);
	static QPolygonF changeRectBeginEnd(QPolygonF polygon);
	static QPointF getLTPointFromCenter(QPointF center, int width, int height);
	static QPointF getPointImageToPatchFromCenter(QPointF point, QPointF center, int width, int height);
	static QPointF getPointScale(QPointF point, int originalWidth, int originalHeight, int newWidth, int newHeight);
	static QRectF updateRectFromCenter(QPointF center, int width, int height);
	static std::vector<std::vector<int>> getClipBoxList(int width, int height, int clipWidth, int clipHeight, int overlap, bool direction);
	static std::vector<std::vector<int>> getRecoverList(std::vector<std::vector<int>> clipBoxList, int baseImageWidth, int baseImageHeight, int Width, int Height, int overlap);

	// basic image processing functions
	static cv::Mat mergeHeight(cv::Mat A, cv::Mat B);  //merge in col
	static cv::Mat mergeWidth(cv::Mat A, cv::Mat B);  //merge in row
};