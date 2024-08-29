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

	QComboBox* bandComboBox; // ������������ʾ���в���
	QLabel* redLabel; // ��ǩ��ʾ��ɫ����ѡ��
	QLabel* greenLabel; // ��ǩ��ʾ��ɫ����ѡ��
	QLabel* blueLabel; // ��ǩ��ʾ��ɫ����ѡ��
	int bandCount=3; // ��������
	int selectedRedBandIndex = 0; // ��ɫ���ε�����
	int selectedGreenBandIndex = 0; // ��ɫ���ε�����
	int selectedBlueBandIndex = 0; // ��ɫ���ε�����

public slots:
	void setRedBand();
	void setGreenBand();
	void setBlueBand();
private:
	Ui::InterfaceClass ui;
};
