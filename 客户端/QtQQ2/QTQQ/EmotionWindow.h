#pragma once

#include <QWidget>
#include "ui_EmotionWindow.h"

class EmotionWindow : public QWidget
{
	Q_OBJECT

public:
	EmotionWindow(QWidget* parent = Q_NULLPTR);
	~EmotionWindow();

private:
	void initControl();

private slots:
	void addEmotion(int emotionNum);			// ��ӱ��飬���Ǳ������



signals:
	// ���鴰�����أ���ѡ�е�ʱ�������
	void signalEmotionWindowHide();
	// ѡ�б���
	void signalEmotionItemClicked(int emotionNum);


private:
	void paintEvent(QPaintEvent* event) override;		// ��д��ͼ�¼�


private:
	Ui::EmotionWindow ui;
};
