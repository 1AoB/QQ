#pragma once

#include <QWidget>
#include "ui_TalkWindowItem.h"

class TalkWindowItem : public QWidget
{
	Q_OBJECT

public:
	TalkWindowItem(QWidget* parent = Q_NULLPTR);
	~TalkWindowItem();

	void setHeadPixmap(const QPixmap& pixmap);
	void setMsgLabelContent(const QString& msg);
	QString getMsgLabelText();			// 获取在跟谁聊天，返回字符串

private:
	void enterEvent(QEvent* event);							// 进入事件
	void leaveEvent(QEvent* event);							// 离开事件
	void resizeEvent(QResizeEvent* event);				// 尺寸进行 resize


private:
	void initControl();


signals:
	void signalCloseClicked();			// 关闭信号

private:
	Ui::TalkWindowItem ui;
};
