#include "WindowManager.h"
#include "TalkWindow.h"
#include "TalkWindowItem.h"

#include <QSqlQueryModel>

// ����ģʽ������ȫ�־�̬����
Q_GLOBAL_STATIC(WindowManager, theInstance)


WindowManager::WindowManager()
	: QObject(nullptr)
	, m_talkwindowshell(nullptr)
{

}

WindowManager::~WindowManager()
{
}

QWidget* WindowManager::findWindowName(const QString& qsWindowName)
{
	// ���ݴ������Ĵ������ƣ������ж�
	// ӳ���У�������� �������Ĵ���
	if (m_windowMap.contains(qsWindowName))
	{
		// ����������ͷ��� m_windowMap ��ֵ
		return m_windowMap.value(qsWindowName);
	}

	return nullptr;			// ���û�оͷ��ؿ�
}

void WindowManager::deleteWindowName(const QString& qsWindowName)
{
	// ��ӳ���У�����ɾ�������ǹرմ���
	// ���ݴ������ƣ�����ɾ��
	m_windowMap.remove(qsWindowName);
}

void WindowManager::addWindowName(const QString& qsWindowName, QWidget* qWidget)
{
	// ��ӳ���У���Ӵ�����
	// ���жϣ�ӳ������û�а��� �������Ĵ�����
	// û�еĻ����ٽ������
	if (!m_windowMap.contains(qsWindowName))
	{
		m_windowMap.insert(qsWindowName, qWidget);
	}
}

WindowManager* WindowManager::getInstance()
{
	// ���ص�ǰʵ��
	return theInstance();
}

TalkWindowShell* WindowManager::getTalkWindowSheel()
{
	return m_talkwindowshell;
}

void WindowManager::addNewTalkWindow(const QString& uid)/*, GroupType groupType, const QString& strPeople*/
{
	// �жϣ��ȱ�֤ �������Ѿ����������ˣ��������
	// ����ǿյģ��͵��ȹ��� ������
	if (m_talkwindowshell == nullptr)
	{
		m_talkwindowshell = new TalkWindowShell;

		// �� TalkWindowShell ���رգ����ٵ�ʱ��
		// �����ָ�� m_talkwindowshell ��Ҳ����һ�����٣�
		connect(m_talkwindowshell, &TalkWindowShell::destroyed, [this](QObject* obj)
			{
				m_talkwindowshell = nullptr;
			});
	}

	// �жϣ����ڵ�����
	// ��������uid��
	// �����˽�ĵĻ������uid������������QQ��
	// �����Ⱥ�ĵĻ������� Ⱥ��
	QWidget* widget = findWindowName(uid);

	// ��ʼ���ж�
	// ������������촰�ڣ��͹���һ�����촰��
	if (!widget)
	{
		// ���Ǿ��� ���ڹ����е����촰�ڣ�
		// ���������� m_strCreatingTalkID��ֵ������uid�Ѿ�����
		m_strCreatingTalkID = uid;

		TalkWindow* talkwindow = new TalkWindow(m_talkwindowshell, uid);/*, groupType*/
		TalkWindowItem* talkwindowItem = new TalkWindowItem(talkwindow);

		// ������촰��ʹ�����ˣ��ٻ�ԭ�� ��ֵ
		m_strCreatingTalkID = "";

		// �ж���Ⱥ�ģ����ǵ���
		QSqlQueryModel sqlDepModel;		// ��ģ�Ͳ�ѯ
		// ����ֻ��һ��uid��ȥ����Ⱥ�￴���ܲ����ѵ���¼�����ѵ���˵����Ⱥ��
		// SELECT department_name,sign FROM tab_departments WHERE departmentID = 2001
		QString strSql = QString("SELECT department_name,sign FROM tab_departments WHERE departmentID = %1").arg(uid);
		// ����ѯģ�ͣ�����SQL���
		sqlDepModel.setQuery(strSql);
		int rows = sqlDepModel.rowCount();

		// ��������ǩ����Ϣ��
		QString strWindowName, strMsgLabel;

		// �ж��к�
		if (rows == 0)			// ����
		{
			// ����uid��QQ�ţ���ѯ Ա�����֣�Ա��ǩ��
			// SELECT employee_name,employee_sign FROM tab_employees WHERE employeeID = 10001
			QString sql = QString("SELECT employee_name,employee_sign FROM tab_employees WHERE employeeID = %1").arg(uid);
			sqlDepModel.setQuery(sql);		// ���ò�ѯ

		}

		// �����ݣ���Ҫ ģ������
		// ����������ǩ������
		QModelIndex indexDepIndex, signIndex;
		indexDepIndex = sqlDepModel.index(0, 0);			// ��0�У���0��
		signIndex = sqlDepModel.index(0, 1);					// ��0�У���1��

		// ��������
		strWindowName = sqlDepModel.data(signIndex).toString();
		// ǩ��
		strMsgLabel = sqlDepModel.data(indexDepIndex).toString();

		// ��������
		talkwindow->setWindowName(strWindowName);
		// �����ϵ����ʾ
		talkwindowItem->setMsgLabelContent(strMsgLabel);
		m_talkwindowshell->addTalkWindow(talkwindow, talkwindowItem, uid);


		/*
		switch (groupType)
		{
		case COMPANY:
		{
			talkwindow->setWindowName(QStringLiteral("�䵱��"));
			talkwindowItem->setMsgLabelContent(QStringLiteral("��˾Ⱥ"));
			break;
		}
		case PERSONELGROUP:
		{
			talkwindow->setWindowName(QStringLiteral("������"));
			talkwindowItem->setMsgLabelContent(QStringLiteral("���²�"));
			break;
		}
		case MARKETGROUP:
		{
			talkwindow->setWindowName(QStringLiteral("������"));
			talkwindowItem->setMsgLabelContent(QStringLiteral("�г���"));
			break;
		}
		case DEVELOPMENTGROUP:
		{
			talkwindow->setWindowName(QStringLiteral("������"));
			talkwindowItem->setMsgLabelContent(QStringLiteral("�з���"));
			break;
		}
		case PTOP:		// һ��һ��˽��
		{
			// ��ʱ�����ñ������֣�����ǩ�����öԷ�������
			talkwindow->setWindowName(QStringLiteral(""));
			talkwindowItem->setMsgLabelContent(strPeople);
			break;
		}
		default:
			break;
		}

		// ������촰��
		m_talkwindowshell->addTalkWindow(talkwindow, talkwindowItem, groupType);
		*/
	}
	else
	{
		// ����߽������ã���������б���Ϊ��ѡ��
		// ͨ����� widget ������������
		// ���� key�����ֵ
		QListWidgetItem* item = m_talkwindowshell->getTalkWindowItemMap().key(widget);
		item->setSelected(true);

		// ���ұ߽������� ��ǰ���촰��
		m_talkwindowshell->setCurrentWidget(widget);
	}

	m_talkwindowshell->show();
	m_talkwindowshell->activateWindow();			// ����Ϊ�����
}

QString WindowManager::getCreatingTalkID()
{
	return m_strCreatingTalkID;
}


