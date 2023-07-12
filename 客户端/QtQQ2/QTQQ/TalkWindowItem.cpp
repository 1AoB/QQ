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
//设置头像图片
void TalkWindowItem::setHeadPixmap(const QPixmap& pixmap)
{
	QPixmap mask = QPixmap(":/Resources/MainWindow/head_mask.png");
	// QPixmap head = QPixmap(":/Resources/MainWindow/girl.png");

	//常引用
	const QPixmap& headpixmap =
		CommonUtils::getRoundImage(pixmap, mask, ui.headlabel->size());

	ui.headlabel->setPixmap(headpixmap);			// 设置头像
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
	__super::enterEvent(event);			// 进行一些其它的，默认的操作
}

void TalkWindowItem::leaveEvent(QEvent* event)
{
	ui.tclosebtn->setVisible(false);
	__super::enterEvent(event);			// 进行一些其它的，默认的操作
}

void TalkWindowItem::resizeEvent(QResizeEvent* event)
{
	__super::resizeEvent(event);			// 进行一些其它的，默认的操作
}

//页面初始化
void TalkWindowItem::initControl()
{
	// 软件刚开始的时候，都是不可见的，所以用false
	// setVisible()，设置 是否可见，true可见，false不可见
	ui.tclosebtn->setVisible(false);

	// 如果点击"关闭按钮"，右边的消息聊天窗口，也要关闭
	connect(ui.tclosebtn, SIGNAL(clicked(bool)), this, SIGNAL(signalCloseClicked()));
}
