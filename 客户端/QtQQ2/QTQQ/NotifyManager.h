#pragma once

#include <QObject>

class NotifyManager : public QObject
{
	Q_OBJECT

public:
	NotifyManager();//Notify֪ͨ Manager������
	~NotifyManager();

//�ź�
signals:
	// �ı���ɫ���ź�
	void signalSkinChanged(const QColor& color);			

public:
	// ��ȡ����ʵ��
	static NotifyManager* getInstance();			
	//֪ͨ�����Ĵ��� ����:Ҫ�ı����ɫ
	void notifyOtherWindowChangeSkin(const QColor& color);


private:
	//����:������ʾ��     instance ���ӣ�ʵ��
	static NotifyManager* instance;


};
