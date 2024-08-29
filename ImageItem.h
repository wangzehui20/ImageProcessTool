#pragma once
#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#include <QString>
#include <QWidget>
#include <QGraphicsItem>

static int clipSize = 256;

class ImageItem : public QWidget {

public:
	ImageItem(QWidget *parent = 0);
	~ImageItem();

	double x_pixel, y_pixel;
	QPointF leftTopPoint;
	QString projection;

	QPointF getPolygonCenter(QPolygonF polygon);
	QPolygonF ogrPolygon2Polygon(OGRPolygon* ogrPolygon);
	QVector<QPointF> getPolygonCenterSet(QVector<QPolygonF> polygons);
	QVector<QPointF> polygon2Point(QPolygonF polygon);
	QVector<QPointF> polygon2Points(QPolygonF polygon);
	QVector<QPolygonF> readShape(QString path);
	void clipImage(QString dstDir, QString imgName, QImage img, QVector<QPointF> centers);
	void setProjAndLeftTop(QString path);
	void writeShape(QVector<QPolygonF> polygons, QString path);
};