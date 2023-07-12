#include "EmotionWindow.h"
//#include "QClickLabel.h"
#include "CommonUtils.h"
#include "EmotionLabelItem.h"

#include <QStyleOption>
#include <QPainter>

const int emotionColumn = 14;			// 列
const int emotionRow = 12;			// 行

EmotionWindow::EmotionWindow(QWidget* parent)
	: QWidget(parent)
{
	// FramelessWindowHint，无边框
	// SubWindow，子窗口
	setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);
	setAttribute(Qt::WA_TranslucentBackground);			// 透明
	setAttribute(Qt::WA_DeleteOnClose);			// 最后一个窗口关闭时，进行资源回收
	ui.setupUi(this);

	initControl();		// 初始化控件

}

EmotionWindow::~EmotionWindow()
{
}

void EmotionWindow::initControl()
{
	// 加载样式
	CommonUtils::loadStyleSheet(this, "EmotionWindow");

	// 遍历加载表情
	for (int row = 0; row < emotionRow; row++)
	{
		for (int column = 0; column < emotionColumn; column++)
		{
			EmotionLabelItem* label = new EmotionLabelItem(this);

			// 添加表情，算法精妙
			label->setEmotionName(row * emotionColumn + column);

			// 发射 点击 信号，添加表情
			connect(label, &EmotionLabelItem::emotionClicked, this, &EmotionWindow::addEmotion);
			// 添加到布局管理器中
			ui.gridLayout->addWidget(label, row, column);
		}
	}

}

void EmotionWindow::addEmotion(int emotionNum)
{
	hide();			// 点击表情之后，窗口进行隐藏
	emit signalEmotionWindowHide();		// 发射，窗口隐藏 信号
	emit signalEmotionItemClicked(emotionNum);		// 发射，点击表情 信号
}

void EmotionWindow::paintEvent(QPaintEvent* event)
{
	QStyleOption opt;				// 风格选项
	opt.init(this);						// 初始化选项

	QPainter painter(this);			// 画家

	// style() 获取当前风格，再进行 drawPrimitive() 绘图
	// drawPrimitive(部件，风格，画家，对象);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

	__super::paintEvent(event);		// 进行一些其它的绘图操作
}


