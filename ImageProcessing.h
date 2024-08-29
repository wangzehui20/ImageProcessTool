#pragma once

#include <QtWidgets/QMainWindow>

#include "ui_ImageProcessing.h"
#include "ImageShow.h"

class ImageProcessing : public QMainWindow
{
	Q_OBJECT

private:
	Ui::ImageProcessingClass ui;
	ImageShow* showWidget = new ImageShow;

public:
	ImageProcessing(QWidget* parent = Q_NULLPTR);

	void build_connect();
	void initBackground();
	void initConfigurations();
	void initForeground();
public slots:
	void exitApplication();
	void on_interface();
};
