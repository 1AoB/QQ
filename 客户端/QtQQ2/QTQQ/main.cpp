//#include "CCMainWindow.h"
#include <QtWidgets/QApplication>
#include "UserLogin.h"



int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	//CCMainWindow w;
	//w.show();

	// Ĭ���� true
	// �ĳ�false���ǵ����һ�����ڵ�رյ�ʱ�򣬲Ż��˳�
	a.setQuitOnLastWindowClosed(false);

	UserLogin* userLogin = new UserLogin;
	userLogin->show();


	return a.exec();
}
