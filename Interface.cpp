#include "Interface.h"
#include <QVBoxLayout>
#include <QPushButton>

Interface::Interface(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

    // 创建布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    // 创建一个 QComboBox 来显示所有可用波段
    bandComboBox = new QComboBox(this);

    for (int i = 1; i <= bandCount; i++) {
        QString band = "Band " + QString::number(i);
        bandComboBox->addItem(band);
    }

    // 添加下拉框到主布局中
    mainLayout->addWidget(new QLabel("Select Band:"));
    mainLayout->addWidget(bandComboBox);

    // 创建标签来显示选择的红色、绿色和蓝色波段
    redLabel = new QLabel("Red Band: None", this);
    greenLabel = new QLabel("Green Band: None", this);
    blueLabel = new QLabel("Blue Band: None", this);

    // 添加标签到主布局中
    mainLayout->addWidget(redLabel);
    mainLayout->addWidget(greenLabel);
    mainLayout->addWidget(blueLabel);

    // 创建按钮用于设置红色、绿色和蓝色波段
    QPushButton* setRedButton = new QPushButton("Set as Red Band", this);
    QPushButton* setGreenButton = new QPushButton("Set as Green Band", this);
    QPushButton* setBlueButton = new QPushButton("Set as Blue Band", this);

    // 添加按钮到主布局中
    mainLayout->addWidget(setRedButton);
    mainLayout->addWidget(setGreenButton);
    mainLayout->addWidget(setBlueButton);

    // 连接按钮的 clicked 信号到相应的槽函数
    connect(setRedButton, SIGNAL(clicked()) , this, SLOT(setRedBand()));
    connect(setGreenButton, SIGNAL(clicked()), this, SLOT(setGreenBand()));
    connect(setBlueButton, SIGNAL(clicked()), this, SLOT(setBlueBand()));

    // 设置主布局
    setLayout(mainLayout);
}

Interface::~Interface()
{}

// 槽函数，用于更新红色波段
void Interface::setRedBand() {
    // 获取当前选择的波段
    int index = bandComboBox->currentIndex();
    // 更新红色波段索引
    selectedRedBandIndex = index;

    // 更新标签
    redLabel->setText("Red Band: " + bandComboBox->itemText(index));
}

// 槽函数，用于更新绿色波段
void Interface::setGreenBand() {
    // 获取当前选择的波段
    int index = bandComboBox->currentIndex();
    // 更新绿色波段索引
    selectedGreenBandIndex = index;

    // 更新标签
    greenLabel->setText("Green Band: " + bandComboBox->itemText(index));
}

// 槽函数，用于更新蓝色波段
void Interface::setBlueBand() {
    // 获取当前选择的波段
    int index = bandComboBox->currentIndex();
    // 更新蓝色波段索引
    selectedBlueBandIndex = index;

    // 更新标签
    blueLabel->setText("Blue Band: " + bandComboBox->itemText(index));
}