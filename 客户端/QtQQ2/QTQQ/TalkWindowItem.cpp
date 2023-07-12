#include "TalkWindowItem.h"
#include "CommonUtils.h"

TalkWindowItem::TalkWindowItem(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	initControl();
}

TalkWindowItem::~TalkWindowItem()
{
}
//����ͷ��ͼƬ
void TalkWindowItem::setHeadPixmap(const QPixmap& pixmap)
{
	QPixmap mask = QPixmap(":/Resources/MainWindow/head_mask.png");
	// QPixmap head = QPixmap(":/Resources/MainWindow/girl.png");

	//������
	const QPixmap& headpixmap =
		CommonUtils::getRoundImage(pixmap, mask, ui.headlabel->size());

	ui.headlabel->setPixmap(headpixmap);			// ����ͷ��
}

void TalkWindowItem::setMsgLabelContent(const QString& msg)
{
	ui.msgLabel->setText(msg);
}

QString TalkWindowItem::getMsgLabelText()
{
	return ui.msgLabel->text();
}

void TalkWindowItem::enterEvent(QEvent* event)
{
	ui.tclosebtn->setVisible(true);
	__super::enterEvent(event);			// ����һЩ�����ģ�Ĭ�ϵĲ���
}

void TalkWindowItem::leaveEvent(QEvent* event)
{
	ui.tclosebtn->setVisible(false);
	__super::enterEvent(event);			// ����һЩ�����ģ�Ĭ�ϵĲ���
}

void TalkWindowItem::resizeEvent(QResizeEvent* event)
{
	__super::resizeEvent(event);			// ����һЩ�����ģ�Ĭ�ϵĲ���
}

//ҳ���ʼ��
void TalkWindowItem::initControl()
{
	// ����տ�ʼ��ʱ�򣬶��ǲ��ɼ��ģ�������false
	// setVisible()������ �Ƿ�ɼ���true�ɼ���false���ɼ�
	ui.tclosebtn->setVisible(false);

	// ������"�رհ�ť"���ұߵ���Ϣ���촰�ڣ�ҲҪ�ر�
	connect(ui.tclosebtn, SIGNAL(clicked(bool)), this, SIGNAL(signalCloseClicked()));
}
