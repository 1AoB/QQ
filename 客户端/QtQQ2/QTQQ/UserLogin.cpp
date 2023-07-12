#include "UserLogin.h"
#include "CCMainWindow.h"

#include <QMessageBox>
#include <qdebug.h>
#include <QtSql\qsqlquery.h>

// 登陆者的QQ号（员工号）
QString gLoginEmployeeID;

UserLogin::UserLogin(QWidget* parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);

	// 将构造函数完善一下
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");

	loadStyleSheet("UserLogin");
	initControl();
}

UserLogin::~UserLogin()
{
}

void UserLogin::initControl()
{
	QLabel* headLabel = new QLabel(this);
	headLabel->setFixedSize(68, 68);

	QPixmap pix(":/Resources/MainWindow/head_mask.png");
	headLabel->setPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/app/logo.ico"), pix, headLabel->size()));
	//headLabel->setPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/girl.png"), pix, headLabel->size()));
	headLabel->move(width() / 2 - 34, ui.titleWidget->height() - 34);

	// 因为是空的, 所以不能直接设置,要先调用函数 转换
	connect(ui.loginBtn, &QPushButton::clicked, this, &UserLogin::onLoginBtnClicked);

	// 判断连接数据库，是否成功
	if (!connectMySql())					// 如果失败
	{
		QMessageBox::information(NULL, QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("连接数据库失败"));

		close();			// 直接关闭
	}




}

bool UserLogin::connectMySql()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setDatabaseName("qtqq");				// 设置数据库名称
	db.setHostName("127.0.0.1");			// 主机名
	db.setUserName("root");					// 数据库，用户名
	db.setPassword("wxn15690578178");				// 数据库，密码
	db.setPort(3306);						// 数据库，端口


	if (db.open())
	{
		return true;
	}
	else
	{
		return false;
	}

}

bool UserLogin::veryfyAccountCode(bool& isAccountLogin, QString& strAccount)
{
	QString strAccountInput = ui.editUserAccount->text();
	QString strCodeInput = ui.editPassword->text();

	// 输入员工号，QQ号
	// 关键字用大写，可读性好一些
	// 在数据库里查询，通过employeeID查询
	//【SELECT code FROM tab_accounts WHERE employeeID = 10001】
	QString strSqlCode = QString("SELECT code FROM tab_accounts WHERE employeeID = %1")
		.arg(strAccountInput);
	QSqlQuery queryEmployeeID(strSqlCode);
	queryEmployeeID.exec();					// 执行

	// 调用first方法，让它指向结果集的第一条
	// 判断是否有结果
	if (queryEmployeeID.first())
	{
		// 数据库中，QQ号对应的密码
		// 查询数据库字段，从第0列开始
		// 把数据库中的 密码值 拿出来，转成 字符串
		QString strCode = queryEmployeeID.value(0).toString();

		if (strCode == strCodeInput)
		{
			// 用账号登陆的话，密码正确，保存账号 给全局变量
			gLoginEmployeeID = strAccountInput;
			
			isAccountLogin = false;					// QQ号登陆成功，这里变 false
			strAccount = strAccountInput;		// QQ号赋值
			return true;
		}
		else
		{
			return false;			// 密码错误
		}
	}

	// 账号登陆
	// 用户输入的账号
	// 在数据库里，如果想要查询字符串，就得加【''】单引号
	// 通过 account 查询
	//【SELECT code,employeeID FROM tab_accounts WHERE account = 'renyi'】
	strSqlCode = QString("SELECT code,employeeID FROM tab_accounts WHERE account = '%1'")
		.arg(strAccountInput);
	QSqlQuery queryAccount(strSqlCode);
	queryAccount.exec();
	if (queryAccount.first())
	{
		// value(0)，是密码
		QString strCode = queryAccount.value(0).toString();

		if (strCode == strCodeInput)
		{
			// 用账号登陆的话，密码正确，保存账号 给全局变量
			// value(1)，才是所要的 QQ号
			gLoginEmployeeID = queryAccount.value(1).toString();

			strAccount = strAccountInput;		// 账号赋值
			isAccountLogin = true;					// 账号登陆，就是true
			return true;
		}
		else
		{
			return false;
		}
	}

	// 如果输入都有问题，就直接返回false
	return false;
}

void UserLogin::onLoginBtnClicked()
{
	bool isAccountLogin;
	// 有可能是账号，或者QQ号
	// isAccountLogin，是true，就是账号
	// isAccountLogin，是false，就是QQ号
	QString strAccount;	

	// 验证
	if (!veryfyAccountCode(isAccountLogin, strAccount))
	{
		QMessageBox::information(NULL, QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("账号或密码有误！"));

		// 密码错误，就把账号和密码清空
		ui.editUserAccount->setText("");
		ui.editPassword->setText("");
		return;
	}

	// 更新登陆状态为"登陆"
	QString strSqlStatus = QString("UPDATE tab_employees SET online = 2 WHERE employeeID = %1").arg(gLoginEmployeeID);
	QSqlQuery queryStatus(strSqlStatus);
	queryStatus.exec();

	// 账号正确，再显示窗口
	close();
	CCMainWindow* mainWindow = new CCMainWindow(strAccount, isAccountLogin);
	mainWindow->show();
}
