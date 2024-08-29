#include "ImageShow.h"
#include "Common.h"

#include <qlayout.h>
#include <QMimeData>
#include <QUrl>
#include <QDir>
#include <QFileDialog>
#include <QTime>
#include <QLabel>

ImageShow::ImageShow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//implement drag event
	this->setAcceptDrops(true);
	setMouseTracking(true);

	setFocusPolicy(Qt::StrongFocus);

	initConfigurations();
}

ImageShow::~ImageShow()
{}

void ImageShow::initConfigurations()
{
	currentImgIndex = -1;
	is_trueColor = true;
	selectBand = new int[3] {1, 2, 3};

	_taskMode = ImageScene::taskMode::move;
	labelMode = ImageScene::labelMouseMode::labelMove;
	inferMode = ImageScene::inferMouseMode::inferMove;
}

void ImageShow::initView(int viewNum) {
	for (int i = 0; i < viewNum; ++i) {
		ImageView* view = new ImageView();
		ImageScene* scene = new ImageScene();
		view->setScene(scene);
		viewList.append(view);
	}
}

void ImageShow::initHLayout() {
	QHBoxLayout* viewHLayout = new QHBoxLayout();
	for (int i = 0; i < viewList.size(); ++i) {
		viewHLayout->addWidget(viewList[i]);
	}
	this->setLayout(viewHLayout);
	for (int i = 0; i < viewList.size(); ++i) {
		viewList[i]->hide();
	}
}

void ImageShow::dragEnterEvent(QDragEnterEvent* event) {
	if (event->mimeData()->hasUrls())
	{
		event->acceptProposedAction();
	}
}

void ImageShow::dropEvent(QDropEvent* event) {
	if (event->mimeData()->hasUrls())
	{
		if (!viewList.isEmpty()) resetWindow();
		urls = event->mimeData()->urls();
		initView(urls.size());
		if (urls.size() > 1) buildConnect(viewList);
		initHLayout();

		for (int i = 0; i < urls.size(); ++i)
		{
			QString filePath = urls[i].toLocalFile();
			if (QFileInfo(filePath).isDir())
			{
				imageFiles = Common::findImageFilesInDirectory(filePath);

				if (i == 0 && currentImgIndex == -1) currentImgIndex = 0;
				displayImage(imageFiles[currentImgIndex], viewList[i]);
			}
			else if (Common::isImageFile(filePath))
			{
				displayImage(filePath, viewList[i]);
				imageFiles = Common::findImageFilesInDirectory(QFileInfo(filePath).dir().path());
				if (i == 0)
				{
					currentImgIndex = imageFiles.indexOf(filePath);
				}
			}
		}
		urls.clear();
	}
}

QImage ImageShow::getImage(QString path) {
	if (path == NULL) {
		QMessageBox::about(NULL, "Error", "Import image path is NULL");
	}
	if (ImageWithProjection::is_proj(path))
	{
		Mat imgMat(ImageWithProjection::readImageWithProj(path, selectBand));
		QImage image(imgMat.data, imgMat.cols, imgMat.rows, imgMat.step, QImage::Format_RGB888);
		return image;
	}
	else
	{
		QImage image(path);
		return image;
	}
}

void ImageShow::displayImage(QString path, ImageView* view) {
	//it should set QGraphicsPixmapItem *. if set QGraphicsPixmapItem, it can not display image
	QGraphicsPixmapItem* imgItem = new QGraphicsPixmapItem(QPixmap::fromImage(getImage(path)));
	dynamic_cast<ImageScene*>(view->scene())->imgBaseItem = imgItem;
	view->scene()->addItem(imgItem);
	view->show();
}

void ImageShow::buildConnect(QVector<ImageView*> viewList) {
	for (int i = 0; i < viewList.size() - 1; ++i) {
		for (int j = i + 1; j < viewList.size(); ++j) {
			//respond wheelEvent
			connect(viewList[i], SIGNAL(wheelImg(QPointF, float)), viewList[j], SLOT(on_wheelImg(QPointF, float)));
			connect(viewList[j], SIGNAL(wheelImg(QPointF, float)), viewList[i], SLOT(on_wheelImg(QPointF, float)));

			//respond moveEvent
			connect(viewList[i], SIGNAL(moveImg(QPointF)), viewList[j], SLOT(on_moveImg(QPointF)));
			connect(viewList[j], SIGNAL(moveImg(QPointF)), viewList[i], SLOT(on_moveImg(QPointF)));

			//respond mouse move
			connect(viewList[i], SIGNAL(virtualMouseMove(QPointF)), viewList[j], SLOT(on_virtualMouseMove(QPointF)));
			connect(viewList[j], SIGNAL(virtualMouseMove(QPointF)), viewList[i], SLOT(on_virtualMouseMove(QPointF)));
		}
	}
}

void ImageShow::refreshScene() {
	for (int i = 0; i < viewList.size(); ++i) {
		viewList[i]->scene()->clear();
		dynamic_cast<ImageScene*>(viewList[i]->scene())->initConfigurations();
	}
}

/***********************************************
			        Slots
************************************************/
void ImageShow::lastImg()
{
	this->refreshScene();
	for (int i = 0; i < viewList.size(); ++i)
	{
		if (i == 0)
			currentImgIndex == 0 ? currentImgIndex = imageFiles.size() - 1 : --currentImgIndex;
		displayImage(imageFiles[currentImgIndex], viewList[i]);
	}
}

void ImageShow::nextImg()
{
	this->refreshScene();
	for (int i = 0; i < viewList.size(); ++i)
	{
		if (i == 0)
			currentImgIndex == imageFiles.size() - 1 ? currentImgIndex = 0 : ++currentImgIndex;
		displayImage(imageFiles[currentImgIndex], viewList[i]);
	}
}

void ImageShow::openImg()
{
	QStringList files = Common::openFile(this, "Select Files", "*.png;; *.jpg;; *.jpeg;; *.gif;; *.bmp;; *.tif;; *.tiff;;");
	if (!files.isEmpty()) {
		if (!viewList.isEmpty()) resetWindow();
		initView(files.size());
		if (files.size() > 1) buildConnect(viewList);
		initHLayout();
		for (int i = 0; i < files.size(); ++i)
		{
			if (QFileInfo(files[i]).isDir())
			{
				imageFiles = Common::findImageFilesInDirectory(files[i]);

				if (i == 0 && currentImgIndex == -1) currentImgIndex = 0;
				displayImage(imageFiles[currentImgIndex], viewList[i]);
			}
			else if (Common::isImageFile(files[i]))
			{
				displayImage(files[i], viewList[i]);
				imageFiles = Common::findImageFilesInDirectory(QFileInfo(files[i]).dir().path());
				if (i == 0)
				{
					currentImgIndex = imageFiles.indexOf(files[i]);
				}
			}
		}
	}
}

void ImageShow::openShp() {
	QStringList shpFiles = Common::openFile(this, "Please choose a shapefile to open", "*.shp;;");
	if (shpFiles.isEmpty()) return;
	QString imageFile = imageFiles[currentImgIndex];
	item.setProjAndLeftTop(imageFile);
	for (int i = 0; i < shpFiles.size(); ++i) 
		dynamic_cast<ImageScene*>(viewList[0]->scene())->displayShp(item.readShape(shpFiles[i]));
}

void ImageShow::resetWindow()
{
	if (!viewList.isEmpty())
	{
		refreshScene();
		viewList.clear();
		//QLayout* existingLayout = ui.centralWidget->layout();
		QLayout* existingLayout = this->layout();
		if (existingLayout) {
			QLayoutItem* item;
			while ((item = existingLayout->takeAt(0)) != nullptr) {
				delete item->widget();
				delete item;
			}
			delete existingLayout;
		}
		update();
	}
	initConfigurations();
}

/***********************************************
               Label Interface
************************************************/
void ImageShow::resetLabelWindow() {
	_taskMode = ImageScene::taskMode::label;
	labelMode = ImageScene::labelMouseMode::labelMove;
	for (int i = 0; i < viewList.size(); ++i)
	{
		dynamic_cast<ImageScene*>(viewList[i]->scene())->setTaskMode(_taskMode);
		dynamic_cast<ImageScene*>(viewList[i]->scene())->setLabelMouseMode(labelMode);
		dynamic_cast<ImageScene*>(viewList[i]->scene())->resetLabelWindow();
	}
}

void ImageShow::clipImg()
{
	QString imgFilePath = Common::openDirectory(this, "Choose file to open image");
	if (imgFilePath.isEmpty()) return;
	QString shpFilePath = Common::openDirectory(this, "Choose file to open shapefile");
	if (shpFilePath.isEmpty()) return;
	QString clippedImgFilePath = Common::openDirectory(this, "Choose file to save clipped image");
	if (clippedImgFilePath.isEmpty()) return;

	QStringList tmpImageFiles = Common::findImageFilesInDirectory(imgFilePath);
	QStringList tmpShpFiles = Common::findVectorFilesInDirectory(shpFilePath);
	for (int i = 0; i < tmpImageFiles.size(); ++i)
	{
		QString tempImgName = QFileInfo(tmpImageFiles[i]).baseName();
		QImage img = this->getImage(tmpImageFiles[i]);
		if (ImageWithProjection::is_proj(tmpImageFiles[i])) item.setProjAndLeftTop(tmpImageFiles[i]);
		for (int j = 0; j < tmpShpFiles.size(); ++j)
		{
			QString tempShpName = QFileInfo(tmpShpFiles[j]).baseName();
			if (tempImgName == tempShpName)
			{
				QVector<QPointF> centers = item.getPolygonCenterSet(item.readShape(tmpShpFiles[j]));
				item.clipImage(clippedImgFilePath, tempImgName, img, centers);
			}
		}
	}
}

void ImageShow::polygonMode() {
	_taskMode = ImageScene::taskMode::label;
	labelMode = ImageScene::labelMouseMode::polygon;
	for (int i = 0; i < viewList.size(); ++i)
	{
		dynamic_cast<ImageScene*>(viewList[i]->scene())->setTaskMode(_taskMode);
		dynamic_cast<ImageScene*>(viewList[i]->scene())->setLabelMouseMode(labelMode);
	}
}

void ImageShow::rectMode() {
	_taskMode = ImageScene::taskMode::label;
	labelMode = ImageScene::labelMouseMode::rect;
	for (int i = 0; i < viewList.size(); ++i)
	{
		dynamic_cast<ImageScene*>(viewList[i]->scene())->setTaskMode(_taskMode);
		dynamic_cast<ImageScene*>(viewList[i]->scene())->setLabelMouseMode(labelMode);
	}
}

void ImageShow::removeShp() {
	dynamic_cast<ImageScene*>(viewList[0]->scene())->removePolygonsAndItems();
}

void ImageShow::saveShp() {
	if (_taskMode == ImageScene::taskMode::move) {
		QMessageBox::about(NULL, "Warning", "Not label mode");
		return;
	}
	QString saveShpFilePath = Common::openDirectory(this, "Choose file to save shape(label)");
	if (saveShpFilePath.isEmpty()) return;
	QString imageFile = imageFiles[currentImgIndex];
	QString saveShpPath = QDir(saveShpFilePath).filePath(QFileInfo(imageFile).baseName() + ".shp");

	if (dynamic_cast<ImageScene*>(viewList[0]->scene())->polygons.size() == 0) {
		QMessageBox::about(NULL, "Warning", "You do not draw any shapefiles");
		return;
	}

	item.setProjAndLeftTop(imageFile);
	item.writeShape(dynamic_cast<ImageScene*>(viewList[0]->scene())->polygons, saveShpPath);
}

/***********************************************
			   Infer Interface
************************************************/
void ImageShow::autoMask()
{
	_taskMode = ImageScene::taskMode::infer;
	inferMode = ImageScene::inferMouseMode::inferAuto;
	for (int i = 0; i < viewList.size(); ++i)
	{
		dynamic_cast<ImageScene*>(viewList[i]->scene())->setTaskMode(_taskMode);
		dynamic_cast<ImageScene*>(viewList[i]->scene())->setInferMouseMode(inferMode);
		dynamic_cast<ImageScene*>(viewList[i]->scene())->getMask();
	}
}

void ImageShow::entireMask()
{
	_taskMode = ImageScene::taskMode::infer;
	inferMode = ImageScene::inferMouseMode::inferEntire;
	for (int i = 0; i < viewList.size(); ++i)
	{
		dynamic_cast<ImageScene*>(viewList[i]->scene())->setTaskMode(_taskMode);
		dynamic_cast<ImageScene*>(viewList[i]->scene())->setInferMouseMode(inferMode);
	}
}

void ImageShow::roiMask()
{
	_taskMode = ImageScene::taskMode::infer;
	inferMode = ImageScene::inferMouseMode::manual;
	for (int i = 0; i < viewList.size(); ++i)
	{
		dynamic_cast<ImageScene*>(viewList[i]->scene())->setTaskMode(_taskMode);
		dynamic_cast<ImageScene*>(viewList[i]->scene())->setInferMouseMode(inferMode);
	}
}

void ImageShow::resetInferWindow()
{
	_taskMode = ImageScene::taskMode::infer;
	inferMode = ImageScene::inferMouseMode::inferMove;
	for (int i = 0; i < viewList.size(); ++i)
	{
		dynamic_cast<ImageScene*>(viewList[i]->scene())->setTaskMode(_taskMode);
		dynamic_cast<ImageScene*>(viewList[i]->scene())->setInferMouseMode(inferMode);
		dynamic_cast<ImageScene*>(viewList[i]->scene())->resetInferWindow();
	}
}

//保存推理结果
void ImageShow::saveMask()
{
	if (_taskMode != ImageScene::taskMode::infer) {
		QMessageBox::about(NULL, "Warning", "Not infer mode");
		return;
	}
	for (int i = 0; i < viewList.size(); ++i)
	{
		QString saveInferFilePath = Common::saveImageFile();
		if (saveInferFilePath.isEmpty()) return;
		cv::Mat maskMat = dynamic_cast<ImageScene*>(viewList[i]->scene())->getMaskMat();
		bool success = cv::imwrite(saveInferFilePath.toStdString(), maskMat);
	}
}