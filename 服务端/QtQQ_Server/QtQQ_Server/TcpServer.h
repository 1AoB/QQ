#pragma once

#include <QTcpServer>
#include <QMessageBox>

class TcpServer : public QTcpServer
{
	Q_OBJECT

public:
	TcpServer(int port);
	~TcpServer();

public:
	bool run();//监听

protected:
	//客户端有新的连接的时
	void incomingConnection(qintptr socketDescriptor);

signals:
	void signalTcpMsgComes(QByteArray&);///////////////发送信息的信号////////////////////////

private slots:
	//处理数据
	void SocketDataProcessing(QByteArray& SendData, int descriptor);
	//断开连接处理
	void SocketDisconnected(int descriptor);
	
private:
	int m_port;//端口号
	QList<QTcpSocket*> m_tcpSocketConnectList;
};
