#include "EmotionLabelItem.h"
#include <QMovie>

EmotionLabelItem::EmotionLabelItem(QWidget* parent)
	: QClickLabel(parent)
{
	initControl();
	// ������飬�����ź�
	connect(this, &QClickLabel::clicked, [this]
		{
			emit emotionClicked(m_emotionName);
		});//lambda���ʽ,[]�ŵ����ⲿ����,()�ŵ�����Ҫ�������Ĳ���,û�������ʡ��,{}�Ǻ�����

}

EmotionLabelItem::~EmotionLabelItem()
{
}

void EmotionLabelItem::setEmotionName(int emotionName)
{
	m_emotionName = emotionName;
	QString imageName = QString(":/Resources/MainWindow/emotion/%1.png").arg(emotionName);

	// ��Ϊ�Ƕ�ͼ������Ҫ���� QMovie
	// QMovie��·������ʽ�����ࣩ;
	m_apngMovie = new QMovie(imageName, "apng", this);

	m_apngMovie->start();			// ��ӳ
	m_apngMovie->stop();			// ��ͣ
	setMovie(m_apngMovie);		// ���ã��Ե�ǰ�ı�ǩ���õ�Ӱ


}

void EmotionLabelItem::initControl()
{
	setAlignment(Qt::AlignCenter);						// ���ж���
	setObjectName("emotionLabelItem");			// ���ö�����
	setFixedSize(32, 32);

}
