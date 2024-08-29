#include "ui_ImageShow.h"
#include "ImageView.h"
#include "ImageScene.h"
#include "ImageItem.h"
#include "ImageWithProjection.h"

#include <QWidget>
#include <QMessageBox>
#include <QDragEnterEvent>

class ImageShow : public QWidget
{
	Q_OBJECT

public:
	ImageShow(QWidget *parent = nullptr);
	~ImageShow();

	bool is_trueColor = true;
	int* selectBand = nullptr;
	int currentImgIndex = -1;  //current image index
	ImageItem item;
	ImageScene::inferMouseMode inferMode = ImageScene::inferMouseMode::inferMove;
	ImageScene::labelMouseMode labelMode = ImageScene::labelMouseMode::labelMove;
	ImageScene::taskMode _taskMode = ImageScene::taskMode::move;
	QList<QUrl> urls;
	QStringList imageFiles;
	QStringList shpFiles;

	QImage getImage(QString path);
	void buildConnect(QVector<ImageView*> viewList);
	void displayImage(QString path, ImageView* view);
	void dragEnterEvent(QDragEnterEvent* event);
	void dropEvent(QDropEvent* event);
	void initConfigurations();
	void initHLayout();
	void initView(int viewNum);
	void refreshScene();

private:
	Ui::ImageShowClass ui;
	QVector<ImageView*> viewList;

public slots:
	void lastImg();
	void nextImg();
	void openImg();
	void openShp();
	void resetWindow();

	//Label Interface
	void resetLabelWindow();
	void clipImg();
	void polygonMode();
	void rectMode();
	void removeShp();
	void saveShp();
	
	//Infer Interface
	void autoMask();
	void entireMask();
	void roiMask();
	void resetInferWindow();
	void saveMask();
};
