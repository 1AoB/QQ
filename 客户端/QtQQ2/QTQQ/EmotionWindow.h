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
	void addEmotion(int emotionNum);			// 添加表情，就是表情序号



signals:
	// 表情窗口隐藏，不选中的时候就隐藏
	void signalEmotionWindowHide();
	// 选中表情
	void signalEmotionItemClicked(int emotionNum);


private:
	void paintEvent(QPaintEvent* event) override;		// 重写绘图事件


private:
	Ui::EmotionWindow ui;
};
