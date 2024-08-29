#include "ImageProcessing.h"
#include "Interface.h"
#include "Common.h"
#include <QHBoxLayout>

ImageProcessing::ImageProcessing(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	this->setWindowIcon(QIcon(QCoreApplication::applicationDirPath() + "/Resources/images/logo.jpg"));
	
	this->setMouseTracking(true);
	//set window close, maximize and minimize button
	this->setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
	this->setFocusPolicy(Qt::StrongFocus);

	initConfigurations();
	initBackground();
	initForeground();
	build_connect();  //build interface button and function connection
}

void ImageProcessing::build_connect()
{
	//---------------------Image and Vector Show----------------------
	connect(ui.actionOpen_Image_File, SIGNAL(triggered()), showWidget, SLOT(openImg()));
	connect(ui.actionOpen_Vector_File, SIGNAL(triggered()), showWidget, SLOT(openShp()));
	connect(ui.actionNext_Image, SIGNAL(triggered()), showWidget, SLOT(nextImg()));
	connect(ui.actionLast_Image, SIGNAL(triggered()), showWidget, SLOT(lastImg()));
	connect(ui.actionReset_Window, SIGNAL(triggered()), showWidget, SLOT(resetWindow()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(exitApplication()));
	//Todo: connect(ui.actionChoose_Band, SIGNAL(triggered()), this, SLOT());

	//-----------------------------Label------------------------------
	connect(ui.actionSave_Shapefile, SIGNAL(triggered()), showWidget, SLOT(saveShp()));
	connect(ui.actionRemove_Shapefile, SIGNAL(triggered()), showWidget, SLOT(removeShp()));
	connect(ui.actionLabel_Clip, SIGNAL(triggered()), showWidget, SLOT(clipImg()));
	connect(ui.actionBrowse, SIGNAL(triggered()), showWidget, SLOT(resetLabelWindow()));
	connect(ui.actionPaint_Polygon, SIGNAL(triggered()), showWidget, SLOT(polygonMode()));
	connect(ui.actionPaint_Rect, SIGNAL(triggered()), showWidget, SLOT(rectMode()));

	//-------------------------Interpretation-------------------------
	connect(ui.actionROI_Mask, SIGNAL(triggered()), showWidget, SLOT(roiMask()));
	connect(ui.actionEntire_Mask, SIGNAL(triggered()), showWidget, SLOT(entireMask()));
	connect(ui.actionAuto_Mask, SIGNAL(triggered()), showWidget, SLOT(autoMask()));
	connect(ui.actionReset_Window_3, SIGNAL(triggered()), showWidget, SLOT(resetInferWindow()));
	connect(ui.actionSave_Mask, SIGNAL(triggered()), showWidget, SLOT(saveMask()));

	//---------------------------Basic Tools--------------------------
	connect(ui.actionChoose_Band, SIGNAL(triggered()), this, SLOT(on_interface()));
}

//set background
void ImageProcessing::initBackground() {
	showWidget->setAutoFillBackground(true);
	ui.menuBar->setAutoFillBackground(true);
	ui.statusBar->setAutoFillBackground(true);

	QPalette centralWidgetPalette = showWidget->palette();
	QPalette menuBarPalette = ui.menuBar->palette();
	QPalette statusBarPalette = ui.statusBar->palette();

	centralWidgetPalette.setColor(QPalette::Background, QColor(39, 39, 39));
	menuBarPalette.setColor(QPalette::Background, QColor(39, 39, 39));
	statusBarPalette.setColor(QPalette::Background, QColor(39, 39, 39));

	showWidget->setPalette(centralWidgetPalette);
	ui.menuBar->setPalette(menuBarPalette);
	ui.statusBar->setPalette(statusBarPalette);

	//load background image
	/*ui.centralWidget->setAutoFillBackground(true);
	QPixmap backGroundImage("./image/background.jpg");
	QPalette backGroundPalette = ui.centralWidget->palette();
	backGroundPalette.setBrush(QPalette::Background, backGroundImage.scaled(this->width(), this->height()));
	ui.centralWidget->setPalette(backGroundPalette);*/
}

void ImageProcessing::initConfigurations()
{
	this->setCentralWidget(showWidget);
}

void ImageProcessing::initForeground() {
	/*ui.menuBar->setStyleSheet("QMenuBar{background-color:white}");*/
}

void ImageProcessing::exitApplication()
{
	QCoreApplication::quit();
}

void ImageProcessing::on_interface()
{
	Interface *interfaceWidget = new Interface(this);
	interfaceWidget->show();
}