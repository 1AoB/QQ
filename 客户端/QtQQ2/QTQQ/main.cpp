//#include "CCMainWindow.h"
#include <QtWidgets/QApplication>
#include "UserLogin.h"



int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	//CCMainWindow w;
	//w.show();

	// 默认是 true
	// 改成false，是当最后一个窗口点关闭的时候，才会退出
	a.setQuitOnLastWindowClosed(false);

	UserLogin* userLogin = new UserLogin;
	userLogin->show();


	return a.exec();
}
