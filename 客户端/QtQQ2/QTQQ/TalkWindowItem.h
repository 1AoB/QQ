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
	QString getMsgLabelText();			// ��ȡ�ڸ�˭���죬�����ַ���

private:
	void enterEvent(QEvent* event);							// �����¼�
	void leaveEvent(QEvent* event);							// �뿪�¼�
	void resizeEvent(QResizeEvent* event);				// �ߴ���� resize


private:
	void initControl();


signals:
	void signalCloseClicked();			// �ر��ź�

private:
	Ui::TalkWindowItem ui;
};
