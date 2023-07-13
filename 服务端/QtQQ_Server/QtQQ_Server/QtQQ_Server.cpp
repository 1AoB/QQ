#include "QtQQ_Server.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QFileDialog>

const int gtcpPort = 8888;
const int gUdpPort = 6666;

QtQQ_Server::QtQQ_Server(QWidget *parent)
    : QDialog(parent)
	, m_pixPath("")
{
    ui.setupUi(this);

	if (!connectMySql())
	{
		QMessageBox::warning(NULL,
			QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("连接数据库失败！"));
		close();
		return;
	}

	setDepNameMap();
	setStatusMap();
	setOnlineMap();

	initComboBoxData();

	m_queryInfoModel.setQuery("SELECT * FROM tab_employees");
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//表格只读
	
	//初始化查询公司群所有员工信息
	m_employeeID = 0;
	m_depID = getCompDepID();
	m_compDepID = m_depID;
	
	updateTableData();

	//定时刷新数据
	m_timer = new QTimer(this);
	m_timer->setInterval(150);//150毫秒更新一次
	m_timer->start();
	connect(m_timer, &QTimer::timeout, this, &QtQQ_Server::onRefresh);

	initTcpSocket();
	initUdpSocket();
}

void QtQQ_Server::initComboBoxData()
{
	QString itemText;//组合框项的文本

	//获取公司总的部门数
	QSqlQueryModel queryDepModel;
	queryDepModel.setQuery("SELECT * FROM tab_department");
	int depCounts = queryDepModel.rowCount() - 1;//“公司群”不算作公司的分支部门

	for (int i = 0; i < depCounts; i++)
	{
		itemText = ui.employeeDepBox->itemText(i);
		QSqlQuery queryDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg(itemText));
		if (!queryDepID.exec()) {
			qDebug() << "67行出错!";
		}
		if (!queryDepID.first()) {
			qDebug() << QString::fromLocal8Bit("70行出错!");
			qDebug() << itemText;
		}

		//设置员工所属部门组合框的数据为相应的部门QQ号
		ui.employeeDepBox->setItemData(i, queryDepID.value(0).toInt());
	}

	//多一个“公司群”部门
	for (int i = 0; i < depCounts + 1; i++)
	{
		itemText = ui.departmentBox->itemText(i);
		QSqlQuery queryDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg(itemText));
		if (!queryDepID.exec()) {
			qDebug() << "81行出错!";
		}
		if (!queryDepID.first()) {
			qDebug() << QString::fromLocal8Bit("87行出错!");
			qDebug() << itemText;
		}

		//设置部门组合框的数据为相应的部门QQ号
		ui.departmentBox->setItemData(i, queryDepID.value(0).toInt());
	}
}

void QtQQ_Server::initTcpSocket()
{
	m_tcpServer = new TcpServer(gtcpPort);
	m_tcpServer->run();

	//收到tcp客户端发来的信息后进行udp广播
	bool flag = connect(m_tcpServer, &TcpServer::signalTcpMsgComes,
		this, &QtQQ_Server::onUDPbroadMsg);
	if (!flag)
	{
		QMessageBox::information(this, "Tips", "sigal and slot connect fail!");
	}
	else {
		QMessageBox::information(this, "Tips", "sigal and slot connect success!");
	}
}

void QtQQ_Server::initUdpSocket()
{
	m_udpSender = new QUdpSocket(this);
}

void QtQQ_Server::onUDPbroadMsg(QByteArray& btData)
{
	//qDebug() <<"send message:" << btData;
	for (quint16 port = gUdpPort; port < gUdpPort + 200; ++port)
	{
		//qDebug() << "port:" << port;
		//int ret =
		m_udpSender->writeDatagram(btData, btData.size(), QHostAddress::Broadcast, port);
		//qDebug() << "has written Bytes:" << ret;
	}
}

bool QtQQ_Server::connectMySql()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setDatabaseName("qtqq");	//数据库名称
	db.setHostName("localhost");//主机名
	db.setUserName("root");		//用户名
	db.setPassword("wxn15690578178");	//密码
	db.setPort(3306);			//端口

	if (db.open())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void QtQQ_Server::setDepNameMap()
{
	m_depNameMap.insert(QStringLiteral("2001"), QStringLiteral("人事部"));
	m_depNameMap.insert(QStringLiteral("2002"), QStringLiteral("研发部"));
	m_depNameMap.insert(QStringLiteral("2024"), QStringLiteral("策划部"));
	m_depNameMap.insert(QStringLiteral("2023"), QStringLiteral("氛围组"));
}

void QtQQ_Server::setStatusMap()
{
	m_statusMap.insert(QStringLiteral("1"), QStringLiteral("有效"));
	m_statusMap.insert(QStringLiteral("0"), QStringLiteral("已注销"));
}

//设置在线状态
void QtQQ_Server::setOnlineMap()
{
	m_onlineMap.insert(QStringLiteral("1"), QStringLiteral("离线"));
	m_onlineMap.insert(QStringLiteral("2"), QStringLiteral("在线"));
	m_onlineMap.insert(QStringLiteral("3"), QStringLiteral("隐身"));
}

int QtQQ_Server::getCompDepID()
{
	QSqlQuery queryCompDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'")
		.arg(QString::fromLocal8Bit("公司群")));
	if(!queryCompDepID.exec()){
		qDebug() << "169行出错!";
	}
	if (!queryCompDepID.next()) {
		qDebug() << "173行出错!";
	}

	int compDepID = queryCompDepID.value(0).toInt();
	return compDepID;
}

void QtQQ_Server::updateTableData(int depID, int employeeID)
{
	ui.tableWidget->clear();

	if (depID && depID != m_compDepID)//查询部门
	{
		QString bumen = QString("SELECT * FROM tab_employees WHERE departmentID = %1").arg(depID);
		//qDebug() <<"its bumne:" << bumen;
		m_queryInfoModel.setQuery(bumen);
	}
	else if (employeeID)//精确查找
	{
		m_queryInfoModel.setQuery(QString("SELECT * FROM tab_employees WHERE employeeID = %1").arg(employeeID));
	}
	else//查询所有
	{
		m_queryInfoModel.setQuery(QString("SELECT * FROM tab_employees"));
	}

	int rows = m_queryInfoModel.rowCount();			//总行数（总记录数）
	int columns = m_queryInfoModel.columnCount();	//总列数（总字段数）
	
	QModelIndex index;//模型索引

	//设置表格的行数、列数
	ui.tableWidget->setRowCount(rows);
	ui.tableWidget->setColumnCount(columns);

	//设置表头
	QStringList headers;
	headers << QStringLiteral("部门")
			<< QStringLiteral("工号")
			<< QStringLiteral("员工姓名")
			<< QStringLiteral("员工签名")
			<< QStringLiteral("员工状态")
			<< QStringLiteral("员工头像")
			<< QStringLiteral("在线状态");
	ui.tableWidget->setHorizontalHeaderLabels(headers);

	//设置列等宽
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			index = m_queryInfoModel.index(i, j);//行、列
			QString strData = m_queryInfoModel.data(index).toString();//获取i行j列的数据

			//获取字段名称
			QSqlRecord record = m_queryInfoModel.record(i);//当前行的记录
			QString strRecordName = record.fieldName(j);//列

			if (strRecordName == QLatin1String("departmentID"))
			{
				ui.tableWidget->setItem(i, j, new QTableWidgetItem(m_depNameMap.value(strData)));
				continue;
			}
			else if (strRecordName == QLatin1String("status"))
			{
				ui.tableWidget->setItem(i, j, new QTableWidgetItem(m_statusMap.value(strData)));
				continue;
			}
			else if (strRecordName == QLatin1String("online"))
			{
				ui.tableWidget->setItem(i, j, new QTableWidgetItem(m_onlineMap.value(strData)));
				continue;
			}


			ui.tableWidget->setItem(i, j, new QTableWidgetItem(strData));
		}
	}
}

void QtQQ_Server::onRefresh()
{
	updateTableData(m_depID, m_employeeID);
}

void QtQQ_Server::on_queryDepartmentBtn_clicked()
{
	ui.queryIDLineEdit->clear();
	m_employeeID = 0;
	m_depID = ui.departmentBox->currentData().toInt();
	updateTableData(m_depID);
}

void QtQQ_Server::on_queryIDBtn_clicked()
{
	ui.departmentBox->setCurrentIndex(0);
	m_depID = m_compDepID;

	//检测员工QQ号是否输入
	if (!ui.queryIDLineEdit->text().length())
	{
		QMessageBox::information(this,
			QStringLiteral("提示"),
			QStringLiteral("请输入员工QQ号"));
		ui.queryIDLineEdit->setFocus();
		return;
	}

	//获取用户输入的员工QQ号
	int employeeID = ui.queryIDLineEdit->text().toInt();

	//检测输入的员工QQ号合法性
	QSqlQuery queryInfo(QString("SELECT * FROM tab_employees WHERE employeeID = %1").arg(employeeID));
	if (!queryInfo.exec()) {
		qDebug() << "284行出错!";
	}
	if (!queryInfo.next())
	{
		QMessageBox::information(this,
			QStringLiteral("提示"),
			QStringLiteral("请输入正确的员工QQ号！"));
		ui.queryIDLineEdit->setFocus();
		return;
	}
	else
	{
		m_employeeID = employeeID;
	}
}

//注销
void QtQQ_Server::on_logoutBtn_clicked()
{
	ui.queryIDLineEdit->clear();
	ui.departmentBox->setCurrentIndex(0);

	//检测员工QQ号是否输入
	if (!ui.logoutIDLineEdit->text().length())
	{
		QMessageBox::information(this,
			QStringLiteral("提示"),
			QStringLiteral("请输入员工QQ号"));
		ui.logoutIDLineEdit->setFocus();
		return;
	}

	//获取用户输入的员工QQ号
	int employeeID = ui.logoutIDLineEdit->text().toInt();

	//检测输入的员工QQ号合法性
	QSqlQuery queryInfo(QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1").arg(employeeID));
	if (!queryInfo.exec()) {
		qDebug() << "322行出错!";
	}
	if (!queryInfo.next())
	{
		QMessageBox::information(this,
			QStringLiteral("提示"),
			QStringLiteral("请输入正确的员工QQ号！"));
		ui.logoutIDLineEdit->setFocus();
		return;
	}
	else
	{
		//注销操作，更新数据库数据，将员工的status设置为0
		QSqlQuery sqlUpdate(QString("UPDATE tab_employees SET status = 0 WHERE employeeID = %1").arg(employeeID));
		if (!sqlUpdate.exec()) {
			qDebug() << "337行出错!";
		}

		//获取注销员工的姓名
		QString strName = queryInfo.value(0).toString();

		QMessageBox::information(this,
			QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("员工 %1 的企业QQ:%2 已被注销！")
			.arg(strName)
			.arg(employeeID));

		ui.logoutIDLineEdit->clear();
	}
}

void QtQQ_Server::on_selectPictureBtn_clicked()
{ 
	//获取选择的头像路径 
	m_pixPath = QFileDialog::getOpenFileName( 
		this, 
		QString::fromLocal8Bit("选择头像"), 
		".", 
		"*.png;;*.jpg" 
	); 
	 
	if (!m_pixPath.size()) 
	{ 
		return; 
	} 
	 
	//将头像显示到标签 
	QPixmap pixmap; 
	pixmap.load(m_pixPath); 
 
	qreal widthRatio = (qreal)ui.headLabel->width() / (qreal)pixmap.width();
	qreal heightRatio = (qreal)ui.headLabel->height() / (qreal)pixmap.height();
 
	QSize size(pixmap.width() * widthRatio, pixmap.height() * heightRatio);
	ui.headLabel->setPixmap(pixmap.scaled(size));
}

void QtQQ_Server::on_addBtn_clicked()
{
	//检测员工姓名的输入
	QString strName = ui.nameLineEdit->text();
	if (!strName.size())
	{
		QMessageBox::information(this,
			QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("请输入员工姓名！"));
		ui.nameLineEdit->setFocus();
		return;
	}

	//检测员工选择头像
	if (!m_pixPath.size())
	{
		QMessageBox::information(this,
			QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("请选择员工头像路径！"));
		return;
	}

	//数据库插入新的员工数据
	//获取员工QQ号
	QSqlQuery maxEmployeeID("SELECT MAX(employeeID) FROM tab_employees");
	if (!maxEmployeeID.exec()) {
		qDebug() << "405行出错!";
	}
	maxEmployeeID.next();

	int employeeID = maxEmployeeID.value(0).toInt() + 1;

	//员工部门QQ号
	int depID = ui.employeeDepBox->currentData().toInt();

	//图片路径格式设置为 “/”替换为“\”xxx\xxx\xxx.png
	m_pixPath.replace("/", "\\\\");


	QSqlQuery insertSql(QString("INSERT INTO tab_employees(departmentID,employeeID,employee_name,picture) \
		VALUES(%1,%2,'%3','%4')")
	.arg(depID)
	.arg(employeeID)
	.arg(strName)
	.arg(m_pixPath));

	if (!insertSql.exec()) {
		qDebug() << "426行出错!";
	}
	QMessageBox::information(this,
		QString::fromLocal8Bit("提示"),
		QString::fromLocal8Bit("新增员工成功！"));
	m_pixPath = "";
	ui.nameLineEdit->clear();
	ui.headLabel->setText(QStringLiteral("  员工寸照  "));
}

