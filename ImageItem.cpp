#include "ImageItem.h"

#include <QPen>
#include <QMessagebox>
#include <QFileDialog>

ImageItem::ImageItem(QWidget *parent)
	: QWidget(parent) {
	OGRRegisterAll();
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	//CPLSetConfigOption("OGR_GEOMETRY_ACCEPT_UNCLOSED_RING", "NO");
}

ImageItem::~ImageItem() {
}

/*
first we get outer rectangle through these points
then we compute the center of outer rectangle
*/
QPointF ImageItem::getPolygonCenter(QPolygonF polygon) {
	QPointF centerPoint;
	QVector<QPointF> points = polygon2Points(polygon);
	QVector<float> points_x;
	QVector<float> points_y;
	//最后一个点是闭环点，与第一个点相同
	for (int i = 0; i < points.size() - 1; ++i) {
		points_x.append(points[i].x());
		points_y.append(points[i].y());
	}
	auto x_min = std::min_element(std::begin(points_x), std::end(points_x));
	auto x_max = std::max_element(std::begin(points_x), std::end(points_x));
	auto y_min = std::min_element(std::begin(points_y), std::end(points_y));
	auto y_max = std::max_element(std::begin(points_y), std::end(points_y));
	float x_minValue = *x_min;
	float x_maxValue = *x_max;
	float y_minValue = *y_min;
	float y_maxValue = *y_max;
	centerPoint.setX((int)(x_minValue + (x_maxValue - x_minValue) / 2));
	centerPoint.setY((int)(y_minValue + (y_maxValue - y_minValue) / 2));
	return centerPoint;
}

QPolygonF ImageItem::ogrPolygon2Polygon(OGRPolygon* ogrPolygon) {
	QPolygonF transPolygon;
	OGRLinearRing* linearRing = ogrPolygon->getExteriorRing();
	int pointNum = linearRing->getNumPoints();
	OGRRawPoint* points = new OGRRawPoint[pointNum];
	linearRing->getPoints(points, NULL);
	for (int i = 0; i < pointNum; ++i) {
		QPointF temPoint(points[i].x, points[i].y);
		//whether it has projection
		if (projection == NULL) {
			temPoint.setX(temPoint.x());
			temPoint.setY(-temPoint.y());
		}
		//based on left-top point to compute new location of points
		else {
			temPoint.setX((temPoint.x() - leftTopPoint.x()) / x_pixel);
			temPoint.setY((leftTopPoint.y() - temPoint.y()) / y_pixel);
		}
		transPolygon.append(temPoint);
	}
	return transPolygon;
}

QVector<QPointF> ImageItem::getPolygonCenterSet(QVector<QPolygonF> polygons) {
	QVector<QPointF> centerPoints;
	for (int i = 0; i < polygons.size(); ++i) {
		QPointF tempCenter = getPolygonCenter(polygons[i]);
		if (!tempCenter.isNull()) {
			centerPoints.append(tempCenter);
		}
	}
	return centerPoints;
}

QVector<QPointF> ImageItem::polygon2Point(QPolygonF polygon) {
	QVector<QPointF> tempPoints;
	QVector<QPointF>::iterator iter;
	for (iter = polygon.begin(); iter != polygon.end(); ++iter)
	{
		tempPoints.append(*iter);
	}
	return tempPoints;
}

QVector<QPointF> ImageItem::polygon2Points(QPolygonF polygon) {
	QVector<QPointF> points;
	QVector<QPointF>::iterator iter;
	for (iter = polygon.begin(); iter != polygon.end(); ++iter) {
		points.append(*iter);
	}
	return points;
}

QVector<QPolygonF> ImageItem::readShape(QString path) {
	//readPolygons is return parameter
	QVector<QPolygonF> readPolygons;

	QFileInfo shpInfo(path);
	QString shpBaseName = shpInfo.baseName();

	GDALDataset* shpDataset = (GDALDataset*)GDALOpenEx(path.toStdString().c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
	if (shpDataset == NULL) {
		QMessageBox::about(NULL, "Error", "Shapefile path in reading is NULL");
	}

	OGRLayer* layer = shpDataset->GetLayerByName(shpBaseName.toStdString().c_str());
	//the number of layer
	int layerNum = shpDataset->GetLayerCount();

	OGRFeature* feature;

	//assure that start from first layer
	layer->ResetReading();
	while ((feature = layer->GetNextFeature()) != NULL) {
		OGRGeometry* geometry = feature->GetGeometryRef();
		OGRPolygon* ogrPolygon = (OGRPolygon*)geometry;
		readPolygons.append(ogrPolygon2Polygon(ogrPolygon));
		OGRFeature::DestroyFeature(feature);
	}
	if (shpDataset) GDALClose((GDALDatasetH)shpDataset);
	return readPolygons;
}

void ImageItem::clipImage(QString dstDir, QString imgName, QImage img, QVector<QPointF> centers) 
{
	int width = img.width();
	int height = img.height();
	int clipWidth, clipHeight;
	QPointF leftTopPointByCenter;
	QString clippedImgPath;
	//set clipped image size
	width < clipSize ? clipWidth = width : clipWidth = clipSize;
	height < clipSize ? clipHeight = height : clipHeight = clipSize;
	
	//assure left_top point through center
	for (int i = 0; i < centers.size(); ++i) {
		leftTopPointByCenter.setX((int)(centers[i].x()) - clipSize / 2);
		leftTopPointByCenter.setY((int)(centers[i].y()) - clipSize / 2);
		if (centers[i].x() < clipSize / 2) {
			leftTopPointByCenter.setX(0);
		}
		if (centers[i].y() < clipSize / 2) {
			leftTopPointByCenter.setY(0);
		}
		if ((width - centers[i].x()) <= clipSize / 2) {
			leftTopPointByCenter.setX(width - clipSize);
		}
		if ((height - centers[i].y()) <= clipSize / 2) {
			leftTopPointByCenter.setY(height - clipSize);
		}
		QImage clippedImg = img.copy(leftTopPointByCenter.x(), leftTopPointByCenter.y(), clipWidth, clipHeight);
		if (i == 0) {
			clippedImgPath = dstDir + "/" + imgName + ".tif";
		}
		else {
			clippedImgPath = dstDir + "/" + imgName + QString::number(i + 1) + ".tif";
		}
		clippedImg.save(clippedImgPath);
	}	
}

void ImageItem::setProjAndLeftTop(QString path) {
	GDALDataset *imgDataset = (GDALDataset*)GDALOpen(path.toStdString().c_str(), GA_ReadOnly);
	projection = imgDataset->GetProjectionRef();
	double geoTransform[6];
	imgDataset->GetGeoTransform(geoTransform);
	leftTopPoint.setX(geoTransform[0]);
	leftTopPoint.setY(geoTransform[3]);
	x_pixel = qAbs(geoTransform[1]);
	y_pixel = qAbs(geoTransform[5]);
	if (imgDataset) GDALClose(imgDataset);
}

void ImageItem::writeShape(QVector<QPolygonF> polygons, QString dstPath) {
	QFileInfo shpFileInfo(dstPath);
	QString shpName = shpFileInfo.baseName();

	//register shp drive
	QString shpDriverName = "ESRI Shapefile";
	GDALDriver* shpDriver;
	shpDriver = GetGDALDriverManager()->GetDriverByName(shpDriverName.toStdString().c_str());
	if (shpDriver == NULL) {
		QMessageBox::about(NULL, "Error", "ShapeFile driver running error");
		return;
	}

	//creat shapefile
	dstPath.replace("\\", "\\\\");
	GDALDataset* shpDataset = shpDriver->Create(dstPath.toStdString().c_str(), 0, 0, 0, GDT_Unknown, NULL);
	if (shpDataset == NULL) {
		QMessageBox::about(NULL, "Error", "Shapefile path in writing is NULL");
		return;
	}

	//create layer
	OGRLayer* layer;
	OGRSpatialReference spatialReference;
	spatialReference.SetFromUserInput(projection.toStdString().c_str());

	//set layer name
	layer = shpDataset->CreateLayer(shpName.toStdString().c_str(), &spatialReference, wkbPolygon, NULL);
	if (layer == NULL) {
		QMessageBox::about(NULL, "Error", "Create layer running error");
		return;
	}

	//set field in layer
	OGRFieldDefn fieldLength("FieldID", OFTInteger);
	fieldLength.SetWidth(10);
	if (layer->CreateField(&fieldLength) != OGRERR_NONE) {
		QMessageBox::about(NULL, "Error", "Create field running error");
		return;
	}
	OGRFieldDefn fieldName("FieldName", OFTString);
	fieldName.SetWidth(100);
	layer->CreateField(&fieldName);

	for (int i = 0; i < polygons.size(); ++i) {
		//create feature in layer
		OGRFeature* feature = OGRFeature::CreateFeature(layer->GetLayerDefn());
		feature->SetField(0, i);

		QVector<QPointF> points = polygon2Point(polygons[i]);
		OGRLinearRing ring;
		for (int i = 0; i < points.size(); ++i) {
			if (projection == NULL) {
				points[i].setY(-points[i].y());
			}
			else {
				points[i].setX((points[i].x() * x_pixel) + leftTopPoint.x());
				//pay attention to the direction of coordinate y
				points[i].setY(-(points[i].y() * y_pixel) + leftTopPoint.y());
			}
			ring.addPoint(points[i].x(), points[i].y());
		}

		ring.closeRings();
		OGRPolygon tempPolygon;
		tempPolygon.addRing(&ring);
		feature->SetGeometry(&tempPolygon);

		if (layer->CreateFeature(feature) != OGRERR_NONE)
		{
			QMessageBox::about(NULL, "Error", "Create feature in layer running error");
			return;
		}
		OGRFeature::DestroyFeature(feature);
	}
	GDALClose(shpDataset);
}