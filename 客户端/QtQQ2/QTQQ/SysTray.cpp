#include "SysTray.h"//ʵ��ϵͳ����,��ʵ���ǽ������С��֮��,�����ٵ�����������صĵ��������ҵ�
#include "CustomMenu.h"//�Զ���˵�

SysTray::SysTray(QWidget* parent)
	: m_parent(parent)
	, QSystemTrayIcon(parent)
{
	initSystemTray();
	show();
}

SysTray::~SysTray()
{
}

void SysTray::initSystemTray()
{
	// ������ʾ�����ŵ�����ͼ���ϣ������ʾ��
	setToolTip(QStringLiteral("��ҵQQ"));

	// ����ͼ��
	setIcon(QIcon(":/Resources/MainWindow/app/logo.ico"));

	// ���ͣ�ͼ�걻�����źţ��������Ǵ�����ԭ��        activated(QSystemTrayIcon::ActivationReasonͼ�걻�����ź�
	// �յ��źţ�����ϵͳ����ͼ��            onIconActivateed(QSystemTrayIcon::ActivationReason))����ϵͳ����ͼ��
	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(onIconActivateed(QSystemTrayIcon::ActivationReason)));
}

//���ϵͳ���̲˵�
void SysTray::addSystrayMenu()
{
	// ָ�� m_parent Ϊ������
	CustomMenu* customMenu = new CustomMenu(m_parent);

	// ���ò˵�ѡ��
	customMenu->addCustomMenu("onShow", ":/Resources/MainWindow/app/logo.ico",
		QStringLiteral("��ʾ"));
	customMenu->addCustomMenu("onQuit", ":/Resources/MainWindow/app/page_close_btn_hover.png",
		QStringLiteral("�˳�"));

	// �����ź�
	connect(customMenu->getAction("onShow"), SIGNAL(triggered(bool)),//"�˵��ϵİ�ť"��"�ź�"���Ǵ���triggered(bool)
		m_parent, SLOT(onShowNormal(bool)));
	connect(customMenu->getAction("onQuit"), SIGNAL(triggered(bool)),
		m_parent, SLOT(onShowQuit(bool)));

	// �ò˵������¼�ѭ��������������
	// exec���ò˵������¼�ѭ����������
	// QCursor::pos()���ܵ�λ�ã���ǰ��դ��ȫ������ϵ�е�λ��
	customMenu->exec(QCursor::pos());
	delete customMenu;				// �¼������󣬽����ͷ�
	customMenu = nullptr;
}
//����ϵͳ����ͼ��
void SysTray::onIconActivateed(QSystemTrayIcon::ActivationReason reason)
{
	// �������
	// Trigger ���ǵ������
	if (reason == QSystemTrayIcon::Trigger)
	{
		// �ͽ�����ʾ
		m_parent->show();							// ��������ʾ
		m_parent->activateWindow();		// ͬʱ�Ѹ����壬��Ϊ�����
	}
	// ����������� Context ͼ��
	else if (reason == QSystemTrayIcon::Context)
	{
		addSystrayMenu();			// ���ϵͳ�˵�
	}
}