#pragma once
#include <ImageScene.h>

#include <QgraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>

class ImageView : public QGraphicsView {
	
	Q_OBJECT

public:
	ImageView(QWidget *parent = 0);
	~ImageView();

	float scaleUp, scaleDown;
	QPoint lastCursorPos;

signals:
	void moveImg(QPointF cursorTranslation);
	void virtualMouseMove(QPointF point);
	void wheelImg(QPointF viewPos, float scaleIndex);

	public slots:
	void on_moveImg(QPointF cursorTranslation);
	void on_virtualMouseMove(QPointF point);
	void on_wheelImg(QPointF viewPos, float scaleIndex);

protected:
	//implement scale
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent* event);
};