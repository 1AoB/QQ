#pragma once

#include <QWidget>
#include "ui_ContactItem.h"

class ContactItem : public QWidget
{
	Q_OBJECT

public:
	ContactItem(QWidget* parent = Q_NULLPTR);
	~ContactItem();

	void setUserName(const QString& userName);
	void setSignName(const QString& signName);
	void setHeadPixmap(const QPixmap& headPath);
	QString getUserName()const;
	QSize getHeadLabelSize()const;			// 头像尺寸

private:
	void inieContol();				// 初始化控件


private:
	Ui::ContactItem ui;
};
