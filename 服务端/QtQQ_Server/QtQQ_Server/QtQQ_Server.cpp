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
			QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("�������ݿ�ʧ�ܣ�"));
		close();
		return;
	}

	setDepNameMap();
	setStatusMap();
	setOnlineMap();

	initComboBoxData();

	m_queryInfoModel.setQuery("SELECT * FROM tab_employees");
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//���ֻ��
	
	//��ʼ����ѯ��˾Ⱥ����Ա����Ϣ
	m_employeeID = 0;
	m_depID = getCompDepID();
	m_compDepID = m_depID;
	
	updateTableData();

	//��ʱˢ������
	m_timer = new QTimer(this);
	m_timer->setInterval(150);//150�������һ��
	m_timer->start();
	connect(m_timer, &QTimer::timeout, this, &QtQQ_Server::onRefresh);

	initTcpSocket();
	initUdpSocket();
}

void QtQQ_Server::initComboBoxData()
{
	QString itemText;//��Ͽ�����ı�

	//��ȡ��˾�ܵĲ�����
	QSqlQueryModel queryDepModel;
	queryDepModel.setQuery("SELECT * FROM tab_department");
	int depCounts = queryDepModel.rowCount() - 1;//����˾Ⱥ����������˾�ķ�֧����

	for (int i = 0; i < depCounts; i++)
	{
		itemText = ui.employeeDepBox->itemText(i);
		QSqlQuery queryDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg(itemText));
		if (!queryDepID.exec()) {
			qDebug() << "67�г���!";
		}
		if (!queryDepID.first()) {
			qDebug() << QString::fromLocal8Bit("70�г���!");
			qDebug() << itemText;
		}

		//����Ա������������Ͽ������Ϊ��Ӧ�Ĳ���QQ��
		ui.employeeDepBox->setItemData(i, queryDepID.value(0).toInt());
	}

	//��һ������˾Ⱥ������
	for (int i = 0; i < depCounts + 1; i++)
	{
		itemText = ui.departmentBox->itemText(i);
		QSqlQuery queryDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg(itemText));
		if (!queryDepID.exec()) {
			qDebug() << "81�г���!";
		}
		if (!queryDepID.first()) {
			qDebug() << QString::fromLocal8Bit("87�г���!");
			qDebug() << itemText;
		}

		//���ò�����Ͽ������Ϊ��Ӧ�Ĳ���QQ��
		ui.departmentBox->setItemData(i, queryDepID.value(0).toInt());
	}
}

void QtQQ_Server::initTcpSocket()
{
	m_tcpServer = new TcpServer(gtcpPort);
	m_tcpServer->run();

	//�յ�tcp�ͻ��˷�������Ϣ�����udp�㲥
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
	db.setDatabaseName("qtqq");	//���ݿ�����
	db.setHostName("localhost");//������
	db.setUserName("root");		//�û���
	db.setPassword("wxn15690578178");	//����
	db.setPort(3306);			//�˿�

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
	m_depNameMap.insert(QStringLiteral("2001"), QStringLiteral("���²�"));
	m_depNameMap.insert(QStringLiteral("2002"), QStringLiteral("�з���"));
	m_depNameMap.insert(QStringLiteral("2024"), QStringLiteral("�߻���"));
	m_depNameMap.insert(QStringLiteral("2023"), QStringLiteral("��Χ��"));
}

void QtQQ_Server::setStatusMap()
{
	m_statusMap.insert(QStringLiteral("1"), QStringLiteral("��Ч"));
	m_statusMap.insert(QStringLiteral("0"), QStringLiteral("��ע��"));
}

//��������״̬
void QtQQ_Server::setOnlineMap()
{
	m_onlineMap.insert(QStringLiteral("1"), QStringLiteral("����"));
	m_onlineMap.insert(QStringLiteral("2"), QStringLiteral("����"));
	m_onlineMap.insert(QStringLiteral("3"), QStringLiteral("����"));
}

int QtQQ_Server::getCompDepID()
{
	QSqlQuery queryCompDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'")
		.arg(QString::fromLocal8Bit("��˾Ⱥ")));
	if(!queryCompDepID.exec()){
		qDebug() << "169�г���!";
	}
	if (!queryCompDepID.next()) {
		qDebug() << "173�г���!";
	}

	int compDepID = queryCompDepID.value(0).toInt();
	return compDepID;
}

void QtQQ_Server::updateTableData(int depID, int employeeID)
{
	ui.tableWidget->clear();

	if (depID && depID != m_compDepID)//��ѯ����
	{
		QString bumen = QString("SELECT * FROM tab_employees WHERE departmentID = %1").arg(depID);
		//qDebug() <<"its bumne:" << bumen;
		m_queryInfoModel.setQuery(bumen);
	}
	else if (employeeID)//��ȷ����
	{
		m_queryInfoModel.setQuery(QString("SELECT * FROM tab_employees WHERE employeeID = %1").arg(employeeID));
	}
	else//��ѯ����
	{
		m_queryInfoModel.setQuery(QString("SELECT * FROM tab_employees"));
	}

	int rows = m_queryInfoModel.rowCount();			//���������ܼ�¼����
	int columns = m_queryInfoModel.columnCount();	//�����������ֶ�����
	
	QModelIndex index;//ģ������

	//���ñ�������������
	ui.tableWidget->setRowCount(rows);
	ui.tableWidget->setColumnCount(columns);

	//���ñ�ͷ
	QStringList headers;
	headers << QStringLiteral("����")
			<< QStringLiteral("����")
			<< QStringLiteral("Ա������")
			<< QStringLiteral("Ա��ǩ��")
			<< QStringLiteral("Ա��״̬")
			<< QStringLiteral("Ա��ͷ��")
			<< QStringLiteral("����״̬");
	ui.tableWidget->setHorizontalHeaderLabels(headers);

	//�����еȿ�
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			index = m_queryInfoModel.index(i, j);//�С���
			QString strData = m_queryInfoModel.data(index).toString();//��ȡi��j�е�����

			//��ȡ�ֶ�����
			QSqlRecord record = m_queryInfoModel.record(i);//��ǰ�еļ�¼
			QString strRecordName = record.fieldName(j);//��

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

	//���Ա��QQ���Ƿ�����
	if (!ui.queryIDLineEdit->text().length())
	{
		QMessageBox::information(this,
			QStringLiteral("��ʾ"),
			QStringLiteral("������Ա��QQ��"));
		ui.queryIDLineEdit->setFocus();
		return;
	}

	//��ȡ�û������Ա��QQ��
	int employeeID = ui.queryIDLineEdit->text().toInt();

	//��������Ա��QQ�źϷ���
	QSqlQuery queryInfo(QString("SELECT * FROM tab_employees WHERE employeeID = %1").arg(employeeID));
	if (!queryInfo.exec()) {
		qDebug() << "284�г���!";
	}
	if (!queryInfo.next())
	{
		QMessageBox::information(this,
			QStringLiteral("��ʾ"),
			QStringLiteral("��������ȷ��Ա��QQ�ţ�"));
		ui.queryIDLineEdit->setFocus();
		return;
	}
	else
	{
		m_employeeID = employeeID;
	}
}

//ע��
void QtQQ_Server::on_logoutBtn_clicked()
{
	ui.queryIDLineEdit->clear();
	ui.departmentBox->setCurrentIndex(0);

	//���Ա��QQ���Ƿ�����
	if (!ui.logoutIDLineEdit->text().length())
	{
		QMessageBox::information(this,
			QStringLiteral("��ʾ"),
			QStringLiteral("������Ա��QQ��"));
		ui.logoutIDLineEdit->setFocus();
		return;
	}

	//��ȡ�û������Ա��QQ��
	int employeeID = ui.logoutIDLineEdit->text().toInt();

	//��������Ա��QQ�źϷ���
	QSqlQuery queryInfo(QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1").arg(employeeID));
	if (!queryInfo.exec()) {
		qDebug() << "322�г���!";
	}
	if (!queryInfo.next())
	{
		QMessageBox::information(this,
			QStringLiteral("��ʾ"),
			QStringLiteral("��������ȷ��Ա��QQ�ţ�"));
		ui.logoutIDLineEdit->setFocus();
		return;
	}
	else
	{
		//ע���������������ݿ����ݣ���Ա����status����Ϊ0
		QSqlQuery sqlUpdate(QString("UPDATE tab_employees SET status = 0 WHERE employeeID = %1").arg(employeeID));
		if (!sqlUpdate.exec()) {
			qDebug() << "337�г���!";
		}

		//��ȡע��Ա��������
		QString strName = queryInfo.value(0).toString();

		QMessageBox::information(this,
			QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("Ա�� %1 ����ҵQQ:%2 �ѱ�ע����")
			.arg(strName)
			.arg(employeeID));

		ui.logoutIDLineEdit->clear();
	}
}

void QtQQ_Server::on_selectPictureBtn_clicked()
{ 
	//��ȡѡ���ͷ��·�� 
	m_pixPath = QFileDialog::getOpenFileName( 
		this, 
		QString::fromLocal8Bit("ѡ��ͷ��"), 
		".", 
		"*.png;;*.jpg" 
	); 
	 
	if (!m_pixPath.size()) 
	{ 
		return; 
	} 
	 
	//��ͷ����ʾ����ǩ 
	QPixmap pixmap; 
	pixmap.load(m_pixPath); 
 
	qreal widthRatio = (qreal)ui.headLabel->width() / (qreal)pixmap.width();
	qreal heightRatio = (qreal)ui.headLabel->height() / (qreal)pixmap.height();
 
	QSize size(pixmap.width() * widthRatio, pixmap.height() * heightRatio);
	ui.headLabel->setPixmap(pixmap.scaled(size));
}

void QtQQ_Server::on_addBtn_clicked()
{
	//���Ա������������
	QString strName = ui.nameLineEdit->text();
	if (!strName.size())
	{
		QMessageBox::information(this,
			QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("������Ա��������"));
		ui.nameLineEdit->setFocus();
		return;
	}

	//���Ա��ѡ��ͷ��
	if (!m_pixPath.size())
	{
		QMessageBox::information(this,
			QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("��ѡ��Ա��ͷ��·����"));
		return;
	}

	//���ݿ�����µ�Ա������
	//��ȡԱ��QQ��
	QSqlQuery maxEmployeeID("SELECT MAX(employeeID) FROM tab_employees");
	if (!maxEmployeeID.exec()) {
		qDebug() << "405�г���!";
	}
	maxEmployeeID.next();

	int employeeID = maxEmployeeID.value(0).toInt() + 1;

	//Ա������QQ��
	int depID = ui.employeeDepBox->currentData().toInt();

	//ͼƬ·����ʽ����Ϊ ��/���滻Ϊ��\��xxx\xxx\xxx.png
	m_pixPath.replace("/", "\\\\");


	QSqlQuery insertSql(QString("INSERT INTO tab_employees(departmentID,employeeID,employee_name,picture) \
		VALUES(%1,%2,'%3','%4')")
	.arg(depID)
	.arg(employeeID)
	.arg(strName)
	.arg(m_pixPath));

	if (!insertSql.exec()) {
		qDebug() << "426�г���!";
	}
	QMessageBox::information(this,
		QString::fromLocal8Bit("��ʾ"),
		QString::fromLocal8Bit("����Ա���ɹ���"));
	m_pixPath = "";
	ui.nameLineEdit->clear();
	ui.headLabel->setText(QStringLiteral("  Ա������  "));
}

