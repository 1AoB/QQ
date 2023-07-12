#pragma once

#include "QClickLabel.h"


class QMovie;

class EmotionLabelItem : public QClickLabel
{
	Q_OBJECT

public:
	EmotionLabelItem(QWidget* parent);
	~EmotionLabelItem();

	void setEmotionName(int emotionName);		// 设置表情名称



private:
	void initControl();			// 初始化控件


signals:
	void emotionClicked(int emotionNum);			// 表情被点击了


private:
	int m_emotionName;
	QMovie* m_apngMovie;


};
