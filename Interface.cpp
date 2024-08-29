#include "Interface.h"
#include <QVBoxLayout>
#include <QPushButton>

Interface::Interface(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

    // ��������
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    // ����һ�� QComboBox ����ʾ���п��ò���
    bandComboBox = new QComboBox(this);

    for (int i = 1; i <= bandCount; i++) {
        QString band = "Band " + QString::number(i);
        bandComboBox->addItem(band);
    }

    // �����������������
    mainLayout->addWidget(new QLabel("Select Band:"));
    mainLayout->addWidget(bandComboBox);

    // ������ǩ����ʾѡ��ĺ�ɫ����ɫ����ɫ����
    redLabel = new QLabel("Red Band: None", this);
    greenLabel = new QLabel("Green Band: None", this);
    blueLabel = new QLabel("Blue Band: None", this);

    // ��ӱ�ǩ����������
    mainLayout->addWidget(redLabel);
    mainLayout->addWidget(greenLabel);
    mainLayout->addWidget(blueLabel);

    // ������ť�������ú�ɫ����ɫ����ɫ����
    QPushButton* setRedButton = new QPushButton("Set as Red Band", this);
    QPushButton* setGreenButton = new QPushButton("Set as Green Band", this);
    QPushButton* setBlueButton = new QPushButton("Set as Blue Band", this);

    // ��Ӱ�ť����������
    mainLayout->addWidget(setRedButton);
    mainLayout->addWidget(setGreenButton);
    mainLayout->addWidget(setBlueButton);

    // ���Ӱ�ť�� clicked �źŵ���Ӧ�Ĳۺ���
    connect(setRedButton, SIGNAL(clicked()) , this, SLOT(setRedBand()));
    connect(setGreenButton, SIGNAL(clicked()), this, SLOT(setGreenBand()));
    connect(setBlueButton, SIGNAL(clicked()), this, SLOT(setBlueBand()));

    // ����������
    setLayout(mainLayout);
}

Interface::~Interface()
{}

// �ۺ��������ڸ��º�ɫ����
void Interface::setRedBand() {
    // ��ȡ��ǰѡ��Ĳ���
    int index = bandComboBox->currentIndex();
    // ���º�ɫ��������
    selectedRedBandIndex = index;

    // ���±�ǩ
    redLabel->setText("Red Band: " + bandComboBox->itemText(index));
}

// �ۺ��������ڸ�����ɫ����
void Interface::setGreenBand() {
    // ��ȡ��ǰѡ��Ĳ���
    int index = bandComboBox->currentIndex();
    // ������ɫ��������
    selectedGreenBandIndex = index;

    // ���±�ǩ
    greenLabel->setText("Green Band: " + bandComboBox->itemText(index));
}

// �ۺ��������ڸ�����ɫ����
void Interface::setBlueBand() {
    // ��ȡ��ǰѡ��Ĳ���
    int index = bandComboBox->currentIndex();
    // ������ɫ��������
    selectedBlueBandIndex = index;

    // ���±�ǩ
    blueLabel->setText("Blue Band: " + bandComboBox->itemText(index));
}