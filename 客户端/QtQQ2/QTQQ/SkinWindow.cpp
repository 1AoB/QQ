#include "SkinWindow.h"
#include "QClickLabel.h"
#include "NotifyManager.h"

SkinWindow::SkinWindow(QWidget* parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	loadStyleSheet("SkinWindow");						// 加载样式表
	setAttribute(Qt::WA_DeleteOnClose);				// 当窗体收到关闭事件时候，就会关闭
	initControl();													// 初始化
}

SkinWindow::~SkinWindow()
{
}

void SkinWindow::initControl()
{
	// 先设置 颜色链表
	QList<QColor> colorList = {
		QColor(22,154,218),QColor(40,138,221),QColor(49,166,107),QColor(218,67,68),
		QColor(177,99,158),QColor(107,81,92),QColor(89,92,160),QColor(21,156,199),
		QColor(79,169,172),QColor(155,183,154),QColor(128,77,77),QColor(240,188,189),
	};

	// 再把颜色链表整出来
	for (int row = 0; row < 3; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			// 
			QClickLabel* label = new QClickLabel(this);
			label->setCursor(Qt::PointingHandCursor);

			// C++11，lambda表达式
			// [row,column,colorList](){ } ，信号接收者
			// 这个信号发射的时候直接执行{}里面的代码，
			// 其实{}就是槽函数，只是太懒，槽函数都懒的取名了
			// []里面是捕获的变量
			// QClickLabel.h 中的 信号函数 void clicked(); 
			// 就不要在cpp文件里写了，否则无法运行，因为 信号 是没有函数体的
			connect(label, &QClickLabel::clicked, [row, column, colorList]() {
				NotifyManager::getInstance()->notifyOtherWindowChangeSkin(colorList.at(row * 4 + column));
				});

			label->setFixedSize(84, 84);

			QPalette palette;
			palette.setColor(QPalette::Background, colorList.at(row * 4 + column));
			label->setAutoFillBackground(true);
			label->setPalette(palette);

			ui.gridLayout->addWidget(label, row, column);
		}
	}

	// 最小化，最大化
	connect(ui.sysmin, SIGNAL(clicked(bool)), this, SLOT(onShowMin(bool)));
	connect(ui.sysclose, SIGNAL(clicked()), this, SLOT(onShowClose()));
}


void SkinWindow::onShowClose()
{
	close();
}







