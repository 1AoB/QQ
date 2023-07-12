#pragma once

#include <QLabel>


class QClickLabel : public QLabel
{
	Q_OBJECT

public:
	QClickLabel(QWidget* parent);
	~QClickLabel();


protected:
	// 自定义的，可以点击的标签
	// 鼠标点击时，发射 Clicked点击信号
	void mousePressEvent(QMouseEvent* event);

signals:
	void clicked();



};
