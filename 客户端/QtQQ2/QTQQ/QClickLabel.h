#pragma once

#include <QLabel>


class QClickLabel : public QLabel
{
	Q_OBJECT

public:
	QClickLabel(QWidget* parent);
	~QClickLabel();


protected:
	// �Զ���ģ����Ե���ı�ǩ
	// �����ʱ������ Clicked����ź�
	void mousePressEvent(QMouseEvent* event);

signals:
	void clicked();



};
