#pragma once

#include "BasicWindow.h"
#include "ui_CCMainWindow.h"


class QTreeWidgetItem;

class CCMainWindow : public BasicWindow
{
	Q_OBJECT

public:
	// 如果isAccountLogin返回的是 true，说明 account 是账号登陆的
	// 如果是false，说明 account 是 QQ号
	CCMainWindow(QString account, bool isAccountLogin, QWidget* parent = Q_NULLPTR);
	~CCMainWindow();

public:
	// 初始化控件
	void initControl();
	void initTimer();				// 初始化计时器
	void setUserName(const QString& username);		// 设置用户名
	void setLevelPixmap(int level);								// 设置等级图片
	void setHeadPixmap(const QString& headPath);	// 设置头像路径
	void setStatusMenuIcon(const QString& statusPath);		// 设置状态

	// 添加应用部件(app图片路径，app部件对象名)
	QWidget* addOtherAppExtension(const QString& appPath, const QString& appName);

	// 初始化联系人
	void initContacTree();
	// （，传入部门ID）
	void addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepID);

private:
	// 当窗口改变时候，用户名前后要保持一致
	// 通过这个事件实现
	void resizeEvent(QResizeEvent* event);

	// 重写 事件过滤器
	bool eventFilter(QObject* obj, QEvent* event);

	// 重写 鼠标按下事件
	void mousePressEvent(QMouseEvent* event);

	// 更新搜索框样式
	void updateSeachStyle();

private slots:
	void onAppIconClicked();		//更换皮肤

	void onItemClicked(QTreeWidgetItem* item, int column);			// 单击，点击
	void onItemExpanded(QTreeWidgetItem* item);							// 展开
	void onItemCollapsed(QTreeWidgetItem* item);							// 收缩
	void onItemDoubleClicked(QTreeWidgetItem* item, int column);		// 双击

	QString getHeadPicPath();		// 获取头像路径


private:
	Ui::CCMainWindowClass ui;

	// 有可能是账号，或者QQ号
	// isAccountLogin，是true，就是账号
	// isAccountLogin，是false，就是QQ号
	bool m_isAccountLogin;
	QString m_account;


	// 所有分组的项
	// 每一个聊天窗口，多对应的名称
	// <公司群和人事部啥的项 ， 值就是ID>
	// 连接数据库之后，这个映射就不需要了
	// QMap<QTreeWidgetItem*, QString> m_groupMap;
};