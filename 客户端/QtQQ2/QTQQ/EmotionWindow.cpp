#include "EmotionWindow.h"
//#include "QClickLabel.h"
#include "CommonUtils.h"
#include "EmotionLabelItem.h"

#include <QStyleOption>
#include <QPainter>

const int emotionColumn = 14;			// ��
const int emotionRow = 12;			// ��

EmotionWindow::EmotionWindow(QWidget* parent)
	: QWidget(parent)
{
	// FramelessWindowHint���ޱ߿�
	// SubWindow���Ӵ���
	setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);
	setAttribute(Qt::WA_TranslucentBackground);			// ͸��
	setAttribute(Qt::WA_DeleteOnClose);			// ���һ�����ڹر�ʱ��������Դ����
	ui.setupUi(this);

	initControl();		// ��ʼ���ؼ�

}

EmotionWindow::~EmotionWindow()
{
}

void EmotionWindow::initControl()
{
	// ������ʽ
	CommonUtils::loadStyleSheet(this, "EmotionWindow");

	// �������ر���
	for (int row = 0; row < emotionRow; row++)
	{
		for (int column = 0; column < emotionColumn; column++)
		{
			EmotionLabelItem* label = new EmotionLabelItem(this);

			// ��ӱ��飬�㷨����
			label->setEmotionName(row * emotionColumn + column);

			// ���� ��� �źţ���ӱ���
			connect(label, &EmotionLabelItem::emotionClicked, this, &EmotionWindow::addEmotion);
			// ��ӵ����ֹ�������
			ui.gridLayout->addWidget(label, row, column);
		}
	}

}

void EmotionWindow::addEmotion(int emotionNum)
{
	hide();			// �������֮�󣬴��ڽ�������
	emit signalEmotionWindowHide();		// ���䣬�������� �ź�
	emit signalEmotionItemClicked(emotionNum);		// ���䣬������� �ź�
}

void EmotionWindow::paintEvent(QPaintEvent* event)
{
	QStyleOption opt;				// ���ѡ��
	opt.init(this);						// ��ʼ��ѡ��

	QPainter painter(this);			// ����

	// style() ��ȡ��ǰ����ٽ��� drawPrimitive() ��ͼ
	// drawPrimitive(��������񣬻��ң�����);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

	__super::paintEvent(event);		// ����һЩ�����Ļ�ͼ����
}


