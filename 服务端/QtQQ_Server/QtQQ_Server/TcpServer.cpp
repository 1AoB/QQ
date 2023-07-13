#include "TcpServer.h"
#include <QTcpSocket>
#include "TcpSocket.h"


TcpServer::TcpServer(int port):m_port(port)
{
	
}

TcpServer::~TcpServer()
{
}

bool TcpServer::run()
{
	if (this->listen(QHostAddress::AnyIPv4, m_port))
	{
		qDebug() << QString::fromLocal8Bit("服务端监听端口 %1 成功！").arg(m_port);
		return true;
	}
	else
	{
		qDebug() << QString::fromLocal8Bit("服务端监听端口 %1 失败!").arg(m_port);
		return false;
	}
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
	qDebug() << QString::fromLocal8Bit("新的连接：") << socketDescriptor << endl;

	TcpSocket* tcpsocket = new TcpSocket();
	tcpsocket->setSocketDescriptor(socketDescriptor);
	tcpsocket->run();

	//收到客户端数据后，server进行处理
	connect(tcpsocket, SIGNAL(signalGetDataFromClient(QByteArray&, int)),
		this, SLOT(SocketDataProcessing(QByteArray&, int)));
	//收到客户端断开连接后，server进行处理
	connect(tcpsocket, SIGNAL(signalClientDisconnect(int)),
		this, SLOT(SocketDisconnected(int)));

	//将socket添加到链表中
	m_tcpSocketConnectList.append(tcpsocket);
}

void TcpServer::SocketDisconnected(int descriptor)
{
	for (int i = 0; i < m_tcpSocketConnectList.count(); ++i)
	{
		QTcpSocket *item = m_tcpSocketConnectList.at(i);
		int itemDescriptor = item->socketDescriptor();

		//查找断开连接的socket
		if (itemDescriptor == descriptor || itemDescriptor == -1)
		{
			m_tcpSocketConnectList.removeAt(i);//断开的socket从链表中移除
			item->deleteLater();//回收资源
			qDebug() << QString::fromLocal8Bit("TcpSocket断开连接：") << descriptor << endl;
			return;
		}
	}
}

void TcpServer::SocketDataProcessing(QByteArray& SendData, int descriptor)
{
	
	for (int i = 0; i < m_tcpSocketConnectList.count(); ++i)
	{
		QTcpSocket *item = m_tcpSocketConnectList.at(i);
		if (item->socketDescriptor() == descriptor)
		{
			//这里有问题
			qDebug() << QString::fromLocal8Bit("来自IP:") << item->peerAddress().toString()
					 << QString::fromLocal8Bit("发来的数据：") << QString(SendData);

			//QMessageBox::information(0, "Tips", "if next code run success,the message will send successfully!");
			qDebug() << QString::fromLocal8Bit("当前员工想要发送的消息是:") << QString(SendData);
			emit signalTcpMsgComes(SendData);
		}
	}
}