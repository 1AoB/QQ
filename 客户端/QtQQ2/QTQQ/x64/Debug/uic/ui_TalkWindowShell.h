/********************************************************************************
** Form generated from reading UI file 'TalkWindowShell.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TALKWINDOWSHELL_H
#define UI_TALKWINDOWSHELL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TalkWindowClass
{
public:
    QVBoxLayout *verticalLayout_2;
    QSplitter *splitter;
    QWidget *leftWidget;
    QVBoxLayout *verticalLayout;
    QListWidget *listWidget;
    QStackedWidget *rightStackedWidget;

    void setupUi(QWidget *TalkWindowClass)
    {
        if (TalkWindowClass->objectName().isEmpty())
            TalkWindowClass->setObjectName(QString::fromUtf8("TalkWindowClass"));
        TalkWindowClass->resize(1043, 730);
        verticalLayout_2 = new QVBoxLayout(TalkWindowClass);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(1, 1, 1, 1);
        splitter = new QSplitter(TalkWindowClass);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setStyleSheet(QString::fromUtf8("QSplitter::handle{\n"
"background-color:white;\n"
"}\n"
"\n"
"QSplitter::handle:horizontal{\n"
"	width:1px;\n"
"}"));
        splitter->setOrientation(Qt::Horizontal);
        splitter->setHandleWidth(1);
        splitter->setChildrenCollapsible(false);
        leftWidget = new QWidget(splitter);
        leftWidget->setObjectName(QString::fromUtf8("leftWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(leftWidget->sizePolicy().hasHeightForWidth());
        leftWidget->setSizePolicy(sizePolicy);
        leftWidget->setMinimumSize(QSize(110, 0));
        leftWidget->setMaximumSize(QSize(248, 16777215));
        verticalLayout = new QVBoxLayout(leftWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        listWidget = new QListWidget(leftWidget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setFrameShape(QFrame::NoFrame);
        listWidget->setProperty("titleskin", QVariant(true));

        verticalLayout->addWidget(listWidget);

        splitter->addWidget(leftWidget);
        rightStackedWidget = new QStackedWidget(splitter);
        rightStackedWidget->setObjectName(QString::fromUtf8("rightStackedWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(rightStackedWidget->sizePolicy().hasHeightForWidth());
        rightStackedWidget->setSizePolicy(sizePolicy1);
        splitter->addWidget(rightStackedWidget);

        verticalLayout_2->addWidget(splitter);


        retranslateUi(TalkWindowClass);

        QMetaObject::connectSlotsByName(TalkWindowClass);
    } // setupUi

    void retranslateUi(QWidget *TalkWindowClass)
    {
        TalkWindowClass->setWindowTitle(QCoreApplication::translate("TalkWindowClass", "TalkWindowShell", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TalkWindowClass: public Ui_TalkWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TALKWINDOWSHELL_H
