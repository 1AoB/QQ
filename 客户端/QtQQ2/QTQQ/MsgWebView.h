#pragma once


#include <QWebEngineView>
#include <QDomNode>
#include <QURL>

class MsgHtmlObj : public QObject
{
	Q_OBJECT

	// ��̬����
	//���������ͣ��������ƣ����Ա�����Ե����Ա���źţ������źţ�
	// m_msgLHtmlTmpl �� m_msgRHtmlTmpl ��������Ҫ ����ſ���
	Q_PROPERTY(QString msgLHtmlTmpl MEMBER m_msgLHtmlTmpl NOTIFY signalMsgHtml)
	Q_PROPERTY(QString msgRHtmlTmpl MEMBER m_msgRHtmlTmpl NOTIFY signalMsgHtml)

public:
	// msgLPicPath������Ϣ�����ˣ���ͷ��·��
	MsgHtmlObj(QObject* parent,QString msgLPicPath = "");


signals:
	void signalMsgHtml(const QString& html);



private:
	void initHtmlTmpl();			// ��ʼ��������ҳ

	// ��ʼ����ҳ��Ϣ�������ǶԷ��ģ������ҵģ���Ҫ��ʼ��
	// ����һ�� code ����������Ϣ�����г�ʼ��
	QString getMsgtmplHtml(const QString& code);

private:
	QString m_msgLHtmlTmpl;			// ���˷�������Ϣ
	QString m_msgRHtmlTmpl;			// �Լ�������Ϣ

	QString m_msgLPicPath;			// ����Ϣ�����ˣ���ͷ��·��
};

// ��д��ҳ�����յ���ҳ��ֻ���� qrc��Դ���������
class MsgWebPage : public QWebEnginePage
{
	Q_OBJECT

public:
	MsgWebPage(QObject* parent = nullptr) : QWebEnginePage(parent)
	{

	}

protected:
	// ���ã�������Ľ������ƣ�ֻ����qrc��Դ���������
	bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame);

};



// QWebEngineView ��ͼ������ ��ʾ��ҳ����
class MsgWebView : public QWebEngineView
{
	Q_OBJECT

public:
	MsgWebView(QWidget *parent);
	~MsgWebView();

	// ׷����Ϣ���� �ַ���
	// strObj��������Ա����QQ��
	// external0 external_xxx
	// ������Լ�����Ϣ����Ĭ����0
	// ������ǣ���QQ��Ȼ����ƴ��
	void appendMsg(const QString& html,QString strObj = "0" ); 


private:
	// ���� html
	QList<QStringList> parseHtml(const QString& html);

	// ���� �ڵ�
	// QT������DOM�ڵ㣨���ԡ�˵�����ı��ȣ��������� QDomNode ���б�ʾ
	QList<QStringList> parseDocNode(const QDomNode& node)
	{
		// ��Ҫ ���ս��������ģ��� �ַ�������
		QList<QStringList> attribute;
		// list ���棬���������ӽڵ�
		const QDomNodeList& list = node.childNodes();

		// list.count()������ĳ���
		for (int i = 0; i < list.count(); i++)
		{
			// ������ȡ����ǰ�����е� �ڵ�
			const QDomNode& node = list.at(i);

			// �ٶԽڵ㣬���н������ж� �Ƿ�Ϊ Ԫ��
			if (node.isElement())
			{
				// toElement() ������ֱ��ת���� Ԫ��
				const QDomElement& element = node.toElement();

				// �ж� Ԫ���� �Ƿ�Ϊ ͼƬ
				if (element.tagName() == "img")
				{
					// ��ȡͼƬ��ֵ
					QStringList attributeList;
					// �õ�ͼƬ��·�������飬��ͷ��
					attributeList << "img" << element.attribute("src");
					attribute << attributeList;
				}

				if (element.tagName() == "span")
				{
					QStringList attributeList;
					attributeList << "text" << element.text();
					attribute << attributeList;
				}

				// ��� ����ڵ� ������ �ӽڵ�
				if (node.hasChildNodes())
				{
					// �� ���������ڵ㣬����֮�� �ٽ���� �浽 attribute
					attribute << parseDocNode(node);
				}
			}
		}

		return attribute;
	}

signals:
	void signalSendMsg(QString& strData, int& msgType, QString sFile = "");


private:
	// ��ҳ��ʾ�Ļ�����Ҫһ������
	// QT��html����ͨ�ţ���Ҫ���� QWeb
	// ��ҳ����
	MsgHtmlObj* m_msgHtmlObj;

	QWebChannel* m_channel;			// ����ͨ��
};
