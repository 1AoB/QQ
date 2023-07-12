#include <QFile>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWebChannel>
#include <QSqlQueryModel>

#include "TalkWindowShell.h"
#include "WindowManager.h"
#include "MsgWebView.h"



extern QString gstrLoginHeadPath;

MsgHtmlObj::MsgHtmlObj(QObject* parent, QString msgLPicPath) 
	: QObject(parent)
{
	m_msgLPicPath = msgLPicPath;
	initHtmlTmpl();
}

void MsgHtmlObj::initHtmlTmpl()
{
	// 初始化 左侧
	// 将msgleftTmpl中的【%1】，替换成 对方头像路径
	m_msgLHtmlTmpl = getMsgtmplHtml("msgleftTmpl");
	m_msgLHtmlTmpl.replace("%1", m_msgLPicPath);

	// 初始化 右侧
	// 将msgrightTmpl中的【%1】，替换成 对方头像路径
	// gstrLoginHeadPath 是 CCMainWindow 中一个全局变量
	m_msgRHtmlTmpl = getMsgtmplHtml("msgrightTmpl");
	m_msgRHtmlTmpl.replace("%1", gstrLoginHeadPath);
}

QString MsgHtmlObj::getMsgtmplHtml(const QString& code)
{
	// 将数据全部读取出来，然后再 返回
	QFile file(":/Resources/MainWindow/MsgHtml/" + code + ".html");

	file.open(QFile::ReadOnly);
	QString strData;

	if (file.isOpen())
	{
		// 全部一次性读取
		strData = QLatin1String(file.readAll());
	}
	else
	{
		// information（父窗体指针，标题，提示内容）;
		// 因为当前写代码的对象，是从 QObject 派生来的，不是从某个部件派生来的
		// 因此要写 空
		QMessageBox::information(nullptr, "Tips", "Failed to init html!");
		return QString("");			// 返回空
	}

	file.close();

	return strData;
}


bool MsgWebPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
	// 仅接受 qrc:/*.html
	// 判断 URL 类型，是否为 qrc类型
	if (url.scheme() == QString("qrc"))
	{
		return true;
	}
	else
	{
		return false;
	}

}


// QWebEngineView 视图，用于 显示网页内容
MsgWebView::MsgWebView(QWidget *parent)
	: QWebEngineView(parent)
	, m_channel(new QWebChannel(this))
{
	// 构造一个网页，并 设置为 当前的网页
	MsgWebPage* page = new MsgWebPage(this);
	this->setPage(page);

	// 通道
	// QWebChannel* channel = new QWebChannel(this);		// 不需要了 注释掉
	m_msgHtmlObj = new MsgHtmlObj(this);

	// 注册
	// "external0" 用于自己发送信息，这个网页对象
	m_channel->registerObject("external0", m_msgHtmlObj);

	TalkWindowShell* talkWindowShell = WindowManager::getInstance()->getTalkWindowSheel();
	connect(this, &MsgWebView::signalSendMsg, talkWindowShell, &TalkWindowShell::updateSendTcpMsg);

	/*
		根据当前打开的聊天窗口，
		在构造聊天窗口的时候，才会构造 MsgWebView 网页视图这个类
		所以要看当前构造的是哪个聊天窗口，
		如果当前是研发部，左侧发信息的对象，只能是研发部的群员

		如果是公司群，那就要将公司群的数据拿过来，根据QQ号 注册对象
	*/
	// 先获取左侧聊天窗口的ID
	// 网页视图 在构造的时候，也就是 聊天窗口 正在构造 时候
	// 当前正构建的聊天窗口的ID（QQ号）
	QString strTalkID = WindowManager::getInstance()->getCreatingTalkID();

	// 创建左侧网页对象，主要根据 ID来创建
	// 2种情况：群聊，单聊。公司群QQ号 2000，属于特殊情况
	// 获取 公司群QQ号，然后做对比
	QSqlQueryModel queryEmployeeModel;
	QString strEmployeeID, strPicturePath;
	QString strExternal;
	bool isGroupTalk = false;		// 群聊标志，初始为 false

	queryEmployeeModel.setQuery(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'")
		.arg(QStringLiteral("公司群")));
	// 获取索引
	QModelIndex companyIndex = queryEmployeeModel.index(0, 0);
	// 通过索引获取 公司群的ID
	QString strCompanyID = queryEmployeeModel.data(companyIndex).toString();
	// 判断，当前窗口ID，是不是公司群
	if (strTalkID == strCompanyID)
	{
		isGroupTalk = true;
		// 如果是公司群窗口，就进数据库遍历
		// 把所有状态为1，没有注销的【员工ID和头像】，全提取出来
		queryEmployeeModel.setQuery("SELECT employeeID,picture FROM tab_employees WHERE status = 1");
	}
	else
	{
		// 不是公司群聊窗口
		// 判断QQ号宽度，群里是2001这种占4位宽度，单聊是10001这种占5位
		if (strTalkID.length() == 4)	// 其他群里
		{
			isGroupTalk = true;
			// 获取 某个群，里面的 群员QQ号
			queryEmployeeModel.setQuery(QString("SELECT employeeID,picture FROM tab_employees WHERE status = 1 AND departmentID = %1")
				.arg(strTalkID));
				
		}
		else  // 单聊
		{
			// 获取 对方QQ号
			queryEmployeeModel.setQuery(QString("SELECT picture FROM tab_employees WHERE status = 1 AND employeeID = %1")
				.arg(strTalkID));
			// 通过索引，找出图片路径，并转成 字符串
			QModelIndex index = queryEmployeeModel.index(0, 0);
			strPicturePath = queryEmployeeModel.data(index).toString();

			strExternal = "external_" + strTalkID;
			
			// 构建网页对象
			MsgHtmlObj* msgHtmlObj = new MsgHtmlObj(this,strPicturePath);
			m_channel->registerObject(strExternal, msgHtmlObj);		// 注册
		}
	}

	// 进行群聊处理
	if (isGroupTalk)
	{
		QModelIndex employeeModelIndex, pictureModelIndex;
		// 模型总行数
		int rows = queryEmployeeModel.rowCount();
		// 遍历群聊
		for (int i = 0; i < rows; i++)
		{
			// 获取索引
			employeeModelIndex = queryEmployeeModel.index(i, 0);	// 群成员QQ号/ID，索引
			pictureModelIndex = queryEmployeeModel.index(i, 1);		// 群成员头像路径，索引

			// 获取群成员QQ号，ID
			strEmployeeID = queryEmployeeModel.data(employeeModelIndex).toString();
			// 获取群成员 头像路径
			strPicturePath = queryEmployeeModel.data(pictureModelIndex).toString();

			strExternal = "external_" + strEmployeeID;

			MsgHtmlObj* msgHtmlObj = new MsgHtmlObj(this, strPicturePath);
			m_channel->registerObject(strExternal, msgHtmlObj);
		}

	}


	// 设置当前网页，网络通道
	this->page()->setWebChannel(m_channel);

	// 加载网页，进行初始化
	this->load(QUrl("qrc:/Resources/MainWindow/MsgHtml/msgTmpl.html"));
}

MsgWebView::~MsgWebView()
{
}

void MsgWebView::appendMsg(const QString& html, QString strObj)
{
	QJsonObject msgObj;
	QString qsMsg;
	const QList<QStringList> msgList = parseHtml(html);		// 解析 html

	// 信息类型，默认给1
	// 0是表情，1是文本，2是文件
	int msgType = 1;
	bool isImageMsg = false;	// 默认的图片信息
	int imageNum = 0;			// 表情数量，默认给0
	QString strData;			// 055，008，表情名称，宽度不够用0补全


	for (int i = 0; i < msgList.size(); i++)
	{
		// 如果 msgList当前循环的，链表的 第一个元素
		// 是 img 文件
		if (msgList.at(i).at(0) == "img")
		{
			// 获得 图片路径
			// 这里写1，0是img，1是图片的真实内容
			QString imagePath = msgList.at(i).at(1);
			QPixmap pixmap;		// 用于加载图片

			// 获取表情名称的位置
			// 表情路径
			QString strEmotionPath = "qrc:/Resources/MainWindow/emotion/";
			int pos = strEmotionPath.size();			// 表情路径长度
			isImageMsg = true;

			// 获取表情名称
			// 通过mid()方法，截取整个字符串中的，部分字符串
			QString strEmotionName = imagePath.mid(pos);
			// 把【.png】用 【空】替换，直接改变的就是 strEmotionName 本身
			strEmotionName.replace(".png", "");

			// 根据表情名称的长度，进行设置表情数据
			// 不足3位，则不足3位，如23.png，则不足为 023.png
			int emotionNameLength = strEmotionName.length();

			// 判断长度
			if (emotionNameLength == 1)
			{
				strData = strData + "00" + strEmotionName;
			}
			else if (emotionNameLength == 2)
			{
				strData = strData + "0" + strEmotionName;
			}
			else if (emotionNameLength == 3)
			{
				strData = strData + strEmotionName;
			}

			msgType = 0;		// 表情信息
			imageNum++;

			// 判断最左边的 3个字符，是否为 qrc
			if (imagePath.left(3) == "qrc")		// 假设路径为 qrc:/MainWindow/xxx
			{
				// mid() 方法，从中间位置开始截取
				// 从第三个字符开始截取，就是 " : " 冒号开始
				// 去掉表情路径中 qrc
				pixmap.load(imagePath.mid(3));
			}
			else
			{	// 不是 qrc的，就直接加载
				pixmap.load(imagePath);
			}

			// 表情图片html格式，文本组合（路径，宽度，高度）
			QString imgPath = QString("<img src=\"%1\" width=\"%2\" height=\"%3\"/>")
				.arg(imagePath).arg(pixmap.width()).arg(pixmap.height());

			// 存到 字符串中
			qsMsg += imgPath;
		}
		else if (msgList.at(i).at(0) == "text")
		{
			// 这里写1，0是text，1是文本的真实内容
			qsMsg += msgList.at(i).at(1);
			strData = qsMsg;				// 赋值
		}
	}

	// 插入到 Json 对象中，是键值对，
	// "MSG"是键，qsMsg是值
	// 体现在，资源文件中，以 msgleftTmpl.html 为例
	// 里面的内容中的 【MSG】 <div class="msg">{{MSG}}<span class="trigon">
	msgObj.insert("MSG", qsMsg);

	// 一切的目的，都是为了 转成QString 字符串类型
	// 再转换成 Json 文档，并且 要转成 UTF-8 的文档
	// QJsonDocument::Compact，紧凑的意思
	const QString& Msg = QJsonDocument(msgObj).toJson(QJsonDocument::Compact);
	
	// 发信息
	if (strObj == "0")
	{
		// 属于 Java脚本语言
		this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));

		// 如果发送的是表情，那就对 发送的数据，进行处理
		if (isImageMsg)
		{
			// 当前strData保存的只是表情的名称，占位是3个宽度
			// 这里加上表情的数量
			strData = QString::number(imageNum) + "images" + strData;
		}

		// 发送信号，发送信息
		emit signalSendMsg(strData, msgType);
	}
	else// 来信
	{
		// 追加数据，这里就是 recvHtml
		this->page()->runJavaScript(QString("recvHtml_%1(%2)").arg(strObj).arg(Msg));
	}


}

QList<QStringList> MsgWebView::parseHtml(const QString& html)
{
	// 因为，传进来的是 html 文件，将它转换成 QT节点文件
	QDomDocument doc;
	doc.setContent(html);					// 转换

	// 想要解析的节点，就是 html里面的 body
	// 想拿到body，需要先获取元素
	// 节点元素
	const QDomElement& root = doc.documentElement();
	// 获取 第一个元素，node 是节点类型，还需要对节点进行 解析
	const QDomNode& node = root.firstChildElement("body");

	return parseDocNode(node);
}

