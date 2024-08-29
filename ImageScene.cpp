#include "ImageScene.h"
#include "Interpretation.h"
#include "Common.h"

#include <iostream>

extern float cross;
extern float crossSize;

ImageScene::ImageScene() {
	imgBaseItem = NULL;
	bufferItem = NULL;
	inferWindowBufferItems = NULL;
	inferBufferItems = NULL;

	initConfigurations();
}

ImageScene::~ImageScene() {

}

cv::Mat ImageScene::getMaskMat()
{
	int sz = inferItems.size();
	if (sz == 0) return cv::Mat();
	cv::Mat maskMat = Common::pixmapToMat(((QGraphicsPixmapItem*)inferItems[sz - 1])->pixmap());
	return maskMat;
}

void ImageScene::displayShp(QVector<QPolygonF> shpPolygons) {
	for (int i = 0; i < shpPolygons.size(); ++i) {
		polygons.append(shpPolygons[i]);
		labelItems.append((QGraphicsItem*)this->addPolygon(polygons[i], initPen, initBrush));
	}
	this->update();
}

void ImageScene::getMask()
{
	QGraphicsPixmapItem* outImageItem;
	Interpretation interpretation;
	cv::Mat slice;
	if (inferMode == manual)
	{
		//删除上一次infer的结果
		if (inferItems.size() > 1)
		{
			for (int i = 1; i < inferItems.size(); ++i)
			{
				removeSceneItems(inferItems[i]);
				inferItems.remove(i);
			}
		}
		slice = Common::getImagePatch(imgBaseItem->pixmap(), inferWindowCenter, inferWindowWidth, inferWindowHeight);
		cv::Mat outImage = interpretation.infer(slice, clickedPoints);

		outImageItem = new QGraphicsPixmapItem(Common::matToPixmap(outImage));
		//设置场景坐标
		outImageItem->setPos(Common::getLTPointFromCenter(inferWindowCenter, inferWindowWidth, inferWindowHeight));
	}
	else if (inferMode == inferEntire || inferMode == inferAuto)
	{
		if (inferItems.size() > 0)
		{
			for (int i = 0; i < inferItems.size(); ++i)
			{
				removeSceneItems(inferItems[i]);
				inferItems.remove(i);
			}
		}
		if (inferMode == inferEntire)
		{
			slice = Common::getImageScale(Common::pixmapToMat(imgBaseItem->pixmap()), inferWindowWidth, inferWindowHeight);
			cv::Mat outImage = interpretation.infer(slice, clickedPoints);
			//再resize回原大小
			QRectF itemBoundingRect = imgBaseItem->sceneBoundingRect();
			cv::Mat outImageResize = Common::getImageScale(outImage, itemBoundingRect.width(), itemBoundingRect.height());
			outImageItem = new QGraphicsPixmapItem(Common::matToPixmap(outImageResize));
		}
			
		else if (inferMode == inferAuto)
		{
			//1.resize为窗口大小推理
			slice = Common::getImageScale(Common::pixmapToMat(imgBaseItem->pixmap()), inferWindowWidth, inferWindowHeight);
			int step = 120;
			cv::Mat outImage = interpretation.infer(slice, step);
			//再resize回原大小
			QRectF itemBoundingRect = imgBaseItem->sceneBoundingRect();
			cv::Mat outImageResize = Common::getImageScale(outImage, itemBoundingRect.width(), itemBoundingRect.height());
			outImageItem = new QGraphicsPixmapItem(Common::matToPixmap(outImageResize));
			
			//2.滑动窗口将底图裁剪为推理窗口大小并逐块推理最后拼接
			//int step = 40;
			//int overlap = std::min(inferWindowWidth, inferWindowHeight) / 2;
			//cv::Mat imgBase = Common::pixmapToMat(imgBaseItem->pixmap());
			//cv::Mat outImage(imgBase.size(), imgBase.type());
			//int imgBaseWidth = imgBaseItem->sceneBoundingRect().width(), imgBaseHeight = imgBaseItem->sceneBoundingRect().height();
			//std::vector<std::vector<int>> clipBoxList = Common::getClipBoxList(imgBaseWidth, imgBaseHeight, inferWindowWidth, inferWindowHeight, overlap, true);
			//std::vector<std::vector<int>> recoverList = Common::getRecoverList(clipBoxList, imgBaseWidth, imgBaseHeight, inferWindowWidth, inferWindowHeight, overlap);
			//for (int i = 0; i < clipBoxList.size(); i++)
			//{
			//	cv::Mat _slice = imgBase.rowRange(clipBoxList[i][0], clipBoxList[i][1]).colRange(clipBoxList[i][2], clipBoxList[i][3]).clone();
			//	int s_x = recoverList[i][4], e_x = recoverList[i][5], s_y = recoverList[i][6], e_y = recoverList[i][7]; //小图的坐标
			//	int r_s_x = recoverList[i][0], r_e_x = recoverList[i][1], r_s_y = recoverList[i][2], r_e_y = recoverList[i][3]; //大图的坐标
			//	if (_slice.cols < inferWindowWidth || _slice.rows < inferWindowHeight)
			//	{
			//		cv::Mat paddedSlice = cv::Mat::zeros(inferWindowHeight, inferWindowWidth, _slice.type());
			//		_slice.copyTo(paddedSlice(cv::Rect(0, 0, _slice.cols, _slice.rows)));
			//		interpretation.infer(paddedSlice, step)(cv::Rect(0, 0, _slice.cols, _slice.rows)).rowRange(s_y, e_y).colRange(s_x, e_x).copyTo(outImage.rowRange(r_s_y, r_e_y).colRange(r_s_x, r_e_x));
			//	}
			//	else
			//	{
			//		interpretation.infer(_slice, step).rowRange(s_y, e_y).colRange(s_x, e_x).copyTo(outImage.rowRange(r_s_y, r_e_y).colRange(r_s_x, r_e_x));
			//	}
			//}
			//outImageItem = new QGraphicsPixmapItem(Common::matToPixmap(outImage));
		}
	}
	this->addItem(outImageItem);
	inferItems.append(outImageItem);
}

void ImageScene::moveVirtualPos(QPointF point) {
	if (hCross || vCross) {
		this->removeItem(hCross);
		this->removeItem(vCross);
	}
	QGraphicsLineItem* hLine = new QGraphicsLineItem(point.x() - cross, point.y(), point.x() + cross, point.y());
	/*hLine->setPen(QPen(Qt::red, crossSize));*/
	hLine->setPen(QPen(QColor(138, 43, 226), crossSize));
	hCross = hLine;
	QGraphicsLineItem* vLine = new QGraphicsLineItem(point.x(), point.y() - cross, point.x(), point.y() + cross);
	/*vLine->setPen(QPen(Qt::red, crossSize));*/
	vLine->setPen(QPen(QColor(138, 43, 226), crossSize));
	vCross = vLine;
	this->addItem(hLine);
	this->addItem(vLine);
	this->update();
}

void ImageScene::initConfigurations()
{
	bufferPen = QPen(QBrush(QColor(253, 130, 124)), 0);
	bufferBrush = QBrush(QColor(169, 169, 169, 80));
	initPen = QPen(QBrush(QColor(69, 137, 148)), 0);
	initBrush = QBrush(QColor(248, 147, 29, 80));
	inferRectPen = QPen(QBrush(QColor(40, 196, 205)), 0);
	inferRectBrush = QBrush(QColor(211, 211, 211, 80));
	//item's scene different
	hCross = new QGraphicsLineItem;
	vCross = new QGraphicsLineItem;
}

void ImageScene::removePolygonsAndItems() {
	int num = labelItems.size();
	if (num == 0) return;
	while (num != 0) {
		this->removeItem(labelItems[num - 1]);
		--num;
	}
	polygons.clear();
	labelItems.clear();
	this->update();
}

void ImageScene::removeSceneItems(QGraphicsItem* item)
{
	if (item != NULL) {
		this->removeItem(item);
		delete item;
		item = NULL;
	}
}

void ImageScene::resetInferWindow()
{
	for (int i = 0; i < inferItems.size(); ++i)
		this->removeItem(inferItems[i]);
	inferItems.clear();
	clickedPoints.clear();
}

void ImageScene::resetLabelWindow()
{
	for (int i = 0; i < labelItems.size(); ++i)
		this->removeItem(labelItems[i]);
	labelItems.clear();
	polygons.clear();
}

void ImageScene::setInferMouseMode(inferMouseMode inferMode)
{
	this->inferMode = inferMode;
}

void ImageScene::setLabelMouseMode(labelMouseMode labelMode)
{
	this->labelMode = labelMode;
}

void ImageScene::setTaskMode(taskMode mode)
{
	_taskMode = mode;
}

void ImageScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
	if (this->items().size() > 0) {
		if (_taskMode == label && labelMode != labelMove)
		{
			if (!bufferPolygon.isEmpty()) {
				removeSceneItems(bufferItem);
				if (labelMode == polygon)
				{
					polygons.append(bufferPolygon);
					labelItems.append((QGraphicsItem*)this->addPolygon(bufferPolygon, bufferPen, bufferBrush));
				}
				else if (labelMode == rect)
				{
					QPolygonF tempPolygon = Common::changeRectBeginEnd(bufferPolygon);
					polygons.append(tempPolygon);
					labelItems.append((QGraphicsItem*)this->addRect(QRectF(tempPolygon[0], tempPolygon[2]), bufferPen, bufferBrush));
				}
				bufferPolygon.clear();
				this->update();
			}
		}
		if (_taskMode == infer && inferMode != inferMove)
		{
			if (event->button() == Qt::LeftButton)
			{
				QPointF currentScenePos = event->scenePos();
				if (!inferItems.isEmpty() || Common::isRectExceed(currentScenePos, imgBaseItem->sceneBoundingRect(), inferWindowWidth, inferWindowHeight)) return;
				removeSceneItems(inferWindowBufferItems);
				inferItems.append((QGraphicsItem*)(this->addRect(Common::updateRectFromCenter(currentScenePos, inferWindowWidth, inferWindowHeight), bufferPen, bufferBrush)));
				inferWindowCenter = currentScenePos;
			}
		}
	}
	QGraphicsScene::mousePressEvent(event);
}

void ImageScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
	if (this->items().size() > 0) {
		if (_taskMode == label && labelMode != labelMove)
		{
			if (!bufferPolygon.isEmpty()) {
				QPointF currentScenePos = event->scenePos();
				QPolygonF tempPolygon(bufferPolygon);
				tempPolygon.append(currentScenePos);
				removeSceneItems(bufferItem);
				if (labelMode == polygon)
					bufferItem = (QGraphicsItem*)(this->addPolygon(tempPolygon, bufferPen, bufferBrush));
				else if (labelMode == rect)
				{
					QPolygonF _tempPolygon = Common::changeRectBeginEnd(tempPolygon);
					bufferItem = (QGraphicsItem*)(this->addRect(QRectF(_tempPolygon[0], _tempPolygon[2]), bufferPen, bufferBrush));
				}
				tempPolygon.clear();
				this->update();
			}
		}
		if (_taskMode == infer && inferMode != inferMove)
		{
			QPointF currentScenePos = event->scenePos();
			if (inferMode == manual && inferItems.isEmpty())
			{
				//infer窗口
				if (inferWindowBufferItems) removeSceneItems(inferWindowBufferItems);
				inferWindowBufferItems = (QGraphicsItem*)(this->addRect(Common::updateRectFromCenter(currentScenePos, inferWindowWidth, inferWindowHeight), bufferPen, bufferBrush));
			}
			if (!inferBufferRect.isEmpty())
			{
				QPolygonF tempPolygon(inferBufferRect);
				tempPolygon.append(currentScenePos);
				removeSceneItems(inferBufferItems);

				QPolygonF _tempPolygon = Common::changeRectBeginEnd(tempPolygon);
				inferBufferItems = (QGraphicsItem*)(this->addRect(QRectF(_tempPolygon[0], _tempPolygon[2]), inferRectPen, inferRectBrush));
			}
		}
	}
	QGraphicsScene::mouseMoveEvent(event);
}

void ImageScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	//taskMode=move会返回，无法点击鼠标
	if (_taskMode == move || (_taskMode == label && labelMode == labelMove) || (_taskMode == infer && inferMode == inferMove)) return;
	if (!event->isAccepted()) {
		if (_taskMode == label)
		{
			if (event->button() == Qt::RightButton)
			{
				QList<QGraphicsItem*> itemsUnderMouse = items(event->scenePos());
				if (!itemsUnderMouse.isEmpty()) {
					QGraphicsItem* itemToRemove = itemsUnderMouse.first();
					for (QGraphicsItem* item : itemsUnderMouse) {
						//itemsUnderMouse[0]和itemsUnderMouse[1]是crossItem，鼠标移动或缩放会重新增加crossItem
						if (item == hCross || item == vCross) continue;
						else {
							itemToRemove = item;
							break;
						}
					}
					if (itemToRemove != imgBaseItem) {
						int removeIndex = labelItems.indexOf(itemToRemove);
						if (removeIndex != -1) {
							// Remove from the scene
							this->removeItem(itemToRemove);

							// Remove from your custom data structure (polygons and labelItems)
							polygons.remove(removeIndex);
							labelItems.remove(removeIndex);

							// Update the view
							this->update();
						}
					}
				}
			}
			else if (event->button() == Qt::LeftButton)
			{
				if (this->items().size() > 0)
				{
					QPointF currentScenePos = event->scenePos();
					if (bufferPolygon.isEmpty()) {
						bufferPolygon.append(currentScenePos);
					}
					else {
						removeSceneItems(bufferItem);
						bufferPolygon.append(currentScenePos);
						if (labelMode == polygon)
							bufferItem = (QGraphicsItem*)(this->addPolygon(bufferPolygon, bufferPen, bufferBrush));
						else if (labelMode == rect)
						{
							//!!!这步应该可以省略
							QPolygonF tempPolygon = Common::changeRectBeginEnd(bufferPolygon);
							bufferItem = (QGraphicsItem*)(this->addRect(QRectF(tempPolygon[0], tempPolygon[2]), bufferPen, bufferBrush));
						}
					}
					this->update();
				}
			}
		}
		else if (_taskMode == infer)
		{
			QPointF currentScenePos = event->scenePos();
			if ((inferMode == manual && !inferItems.isEmpty() && Common::isPointInRectFromCenter(currentScenePos, inferWindowCenter, inferWindowWidth, inferWindowHeight)) || (inferMode == inferEntire && Common::isPointInRect(currentScenePos, imgBaseItem->sceneBoundingRect())))
			{
				int code = -1;
				//负样本点
				if (event->button() == Qt::RightButton) code = 0;
				//正样本点
				else if (event->button() == Qt::LeftButton) code = 2;
				//框
				else if (event->button() == Qt::MidButton && event->modifiers() & Qt::ShiftModifier)
				{
					if (inferBufferRect.size() % 2 == 0)
					{
						code = 4;
						removeSceneItems(inferBufferItems);  //移除已有的框
						inferBufferRect.append(currentScenePos);
					}
					else
					{
						code = 5;
						removeSceneItems(inferBufferItems);
						inferBufferRect.clear();
					}
				}
				if (code >= 0 && code <= 2 && event->modifiers() & Qt::ControlModifier) code += 1;
				if (code >= 0)
				{
					cv::Point3i tmpPoint;
					if (inferMode == manual)
					{
						//转换为相对于选定框的坐标
						QPointF pointInPatch = Common::getPointImageToPatchFromCenter(currentScenePos, inferWindowCenter, inferWindowWidth, inferWindowHeight);
						tmpPoint = { static_cast<int>(pointInPatch.x()), static_cast<int>(pointInPatch.y()), code };
					}
					else if (inferMode == inferEntire)
					{
						QPixmap pixmap = imgBaseItem->pixmap();
						QImage image = pixmap.toImage();
						QPointF pointResize = Common::getPointScale(currentScenePos, image.width(), image.height(), inferWindowWidth, inferWindowHeight);
						tmpPoint = { static_cast<int>(pointResize.x()), static_cast<int>(pointResize.y()), code };
					}
					clickedPoints.push_back(tmpPoint);

					//code如果是rect的第一个点则不进行infer
					if (code != 4) getMask();
					this->update();
				}
			}

		}
	}
	QGraphicsScene::mousePressEvent(event);
}