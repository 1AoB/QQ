#include "RootContactItem.h"
#include <QPainter>

RootContactItem::RootContactItem(bool hasArrow, QWidget* parent)
	: QLabel(parent)
	, m_rotation(0)
	, m_hasArrow(hasArrow)
{
	// ���ù̶��߶�
	setFixedHeight(32);
	// �ߴ磬�������ԣ���չ���ԣ��̶��ģ�
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	// ��ʼ�����Զ���
	// ��rotation����Ϊ����
	m_animation = new QPropertyAnimation(this, "rotation");
	// ���� ���ζ���ʱ����50���� ��ɶ���ʵ��
	m_animation->setDuration(50);
	// ���ö������ߣ�����
	// ���� ������������ ����(t^2)���м��٣����Կ�QT�����ĵ�
	m_animation->setEasingCurve(QEasingCurve::InQuad);

}

RootContactItem::~RootContactItem()
{
}

void RootContactItem::setText(const QString& title)
{
	m_titleText = title;
	update();
}

void RootContactItem::setExpanded(bool expand)
{
	// �ж��Ƿ�չ������ֵ ���ǽǶ�
	if (expand)
	{
		// ���չ��������ֵ����Ϊ 90
		m_animation->setEndValue(90);
	}
	else
	{
		// �������չ��������ֵ����Ϊ 0
		m_animation->setEndValue(0);
	}

	m_animation->start();
}

int RootContactItem::rotation()
{
	return m_rotation;
}

void RootContactItem::setRotation(int rotation)
{
	m_rotation = rotation;			// ���½Ƕ�
	update();
}

void RootContactItem::paintEvent(QPaintEvent* event)
{
	// ������һ������
	QPainter painter(this);			// дthis�����ڵ�ǰ���� ��
	// �����֣�������Ⱦ���
	// �ı������
	painter.setRenderHint(QPainter::TextAntialiasing, true);

	QFont font;
	font.setPointSize(10);
	painter.setFont(font);			// ���������С

	// �����ı�
	// ���� 24, 0����ȣ��߶ȣ�����룬���¿ռ���ж��룬���Ƶ��ı�
	painter.drawText(24, 0, width() - 24, height(), Qt::AlignLeft | Qt::AlignVCenter, m_titleText);
	// ������Ⱦ����ƽ������ת��
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.save();				// ����

	// ����м�ͷ
	if (m_hasArrow)
	{
		// �ͼ���ͼ��
		QPixmap pixmap;
		pixmap.load(":/Resources/MainWindow/arrow.png");

		// ����һ��λͼ������ �м�ͼƬ��Ч��
		QPixmap tmpPixmap(pixmap.size());
		tmpPixmap.fill(Qt::transparent);				// ����͸��

		// �ٴ���һ�����ң�ר���������� ����� tmpPixmap
		QPainter p(&tmpPixmap);
		p.setRenderHint(QPainter::SmoothPixmapTransform, true);

		// ��ת�Ƕ�֮ǰ���ȶ�����ϵ���� ƫ��ת��
		// ��X����ƫ�ƣ�Y����ƫ�ƣ�
		p.translate(pixmap.width() / 2, pixmap.height() / 2);

		// ˳ʱ�룬��ת����ϵ
		// ������ת��m_rotation������ת�Ƕ� ����ȥ
		p.rotate(m_rotation);

		// ��ת֮�������λ�ã�����ͼƬ�����ơ�
		// ��Ϊ֮ǰ���й�����ƫ�ƣ������� 0 ��ȥ�����һ��
		p.drawPixmap(0 - pixmap.width() / 2, 0 - pixmap.height() / 2, pixmap);

		/*
			���� p������ �м�λͼ tmpPixmap ���滭��
			��������Ҫ������һ�� ������ȥ
			�豸�� ��ǰ���ڣ����� this ����
		*/

		// ԭ���Ļ��ң�����λͼ��
		// ����ͼƬ���� p������ tmpPixmap ���滭��ͼ
		painter.drawPixmap(6, (height() - pixmap.height()) / 2, tmpPixmap);

		// ������֮�󣬸����һָ�����
		// ���ǻָ��� save֮ǰ������
		painter.restore();

	}

	// ���еĶ�����֮���ٽ����¼�����
	QLabel::paintEvent(event);
}
