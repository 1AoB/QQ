#include "NotifyManager.h"
#include "CommonUtils.h"

//��ʼ��ʱ���ǿ�ָ��
NotifyManager* NotifyManager::instance = nullptr;

//����
NotifyManager::NotifyManager()
	:QObject(nullptr)
{

}
//���캯��
NotifyManager::~NotifyManager()
{
}
//��õ�ǰ������ʵ��
NotifyManager* NotifyManager::getInstance()
{
	if (instance == nullptr)
	{
		instance = new NotifyManager();
	}

	return instance;
}
//ͨ���������ڽ���Ƥ����ɫ�ĸı�
void NotifyManager::notifyOtherWindowChangeSkin(const QColor& color)
{
	//�����ź�:Ƥ����ɫ�ı�
	emit signalSkinChanged(color);
	// ����Ĭ����ɫ
	CommonUtils::setDefaultSkinColor(color);			
}
