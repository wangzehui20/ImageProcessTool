#pragma once
#include <qgraphicsscene>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QGraphicsPixmapItem>
#include <QList>
#include <QPainter>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

class ImageScene : public QGraphicsScene {
	
	Q_OBJECT

public:
	ImageScene();
	~ImageScene();

	QGraphicsLineItem* hCross;
	QGraphicsLineItem* vCross;
	QGraphicsPixmapItem* imgBaseItem;

	QBrush bufferBrush;
	QBrush inferRectBrush;
	QBrush initBrush;
	QPen bufferPen;
	QPen inferRectPen;
	QPen initPen;
	//buffer polygon and item
	QGraphicsItem* bufferItem;
	QPolygonF bufferPolygon;
	QVector<QGraphicsItem*> labelItems;
	QVector<QPolygonF> polygons;

	//解译
	int inferWindowWidth = 1024;
	int inferWindowHeight = 720;
	QGraphicsItem* inferBufferItems;
	QGraphicsItem* inferWindowBufferItems;  //infer窗口
	QPointF inferWindowCenter;
	QPolygonF inferBufferRect;  //绘制框
	QVector<QGraphicsItem*> inferItems;  //infer窗口、点和框
	std::list<cv::Point3i> clickedPoints;

	enum inferMouseMode { inferMove, manual, inferEntire, inferAuto };
	enum labelMouseMode { labelMove, polygon, rect};
	enum taskMode { move, label, infer};

	cv::Mat getMaskMat();
	//compared with initializePolygons(), this add shapefile into polygons
	void displayShp(QVector<QPolygonF> shpPolygons);
	void getMask();
	void moveVirtualPos(QPointF point);
	void initConfigurations();
	void removePolygonsAndItems();
	void removeSceneItems(QGraphicsItem* bufferItem);
	void resetInferWindow();
	void resetLabelWindow();
	void setInferMouseMode(inferMouseMode inferMode);
	void setLabelMouseMode(labelMouseMode labelMode);
	void setTaskMode(taskMode mode);


protected:
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	void mousePressEvent(QGraphicsSceneMouseEvent* event) Q_DECL_OVERRIDE;

private:
	taskMode _taskMode = taskMode::move;
	inferMouseMode inferMode = inferMouseMode::inferMove;
	labelMouseMode labelMode = labelMouseMode::labelMove;
};