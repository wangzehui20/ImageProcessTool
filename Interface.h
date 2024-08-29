#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include "ui_Interface.h"

class Interface : public QWidget
{
	Q_OBJECT

public:
	Interface(QWidget *parent = nullptr);
	~Interface();

	QComboBox* bandComboBox; // 单个下拉框显示所有波段
	QLabel* redLabel; // 标签显示红色波段选择
	QLabel* greenLabel; // 标签显示绿色波段选择
	QLabel* blueLabel; // 标签显示蓝色波段选择
	int bandCount=3; // 波段数量
	int selectedRedBandIndex = 0; // 红色波段的索引
	int selectedGreenBandIndex = 0; // 绿色波段的索引
	int selectedBlueBandIndex = 0; // 蓝色波段的索引

public slots:
	void setRedBand();
	void setGreenBand();
	void setBlueBand();
private:
	Ui::InterfaceClass ui;
};
