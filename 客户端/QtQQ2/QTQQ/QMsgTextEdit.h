#pragma once

#include <QTextEdit>

class QMsgTextEdit : public QTextEdit
{
	Q_OBJECT
public:
	QMsgTextEdit(QWidget *parent = nullptr);
	~QMsgTextEdit();


public:
	/*
		��Ϊ�������ͼƬ�Ƕ�ͼ������֡�ı��ʱ��
		�ı��༭�����ݵ�λ�ã�ҲҪ��Ӧ�ĸı䡣
	*/

	// ��ӱ��飬�������ID
	void addEmotionUrl(int emotionNum);

	// �����������Ϣʱ��������еı��飬Ҫ��յ���
	void deleteAllEmotionImage();


private slots:
	// �����ı�ʱ����Ӧ�Ĳۺ���
	void onEmotionImageFrameChange(int frame);



private:
	// ��������
	QList<QString> m_listEmotionUrl;

	// ����͵�Ӱ��ӳ�䣬��ϵ
	QMap<QMovie*, QString> m_emotionMap;


};
