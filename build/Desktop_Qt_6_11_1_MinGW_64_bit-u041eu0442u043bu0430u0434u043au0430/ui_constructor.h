/********************************************************************************
** Form generated from reading UI file 'constructor.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONSTRUCTOR_H
#define UI_CONSTRUCTOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Constructor
{
public:
    QWidget *centralwidget;
    QTreeView *m_toFolderView;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Constructor)
    {
        if (Constructor->objectName().isEmpty())
            Constructor->setObjectName("Constructor");
        Constructor->resize(806, 630);
        centralwidget = new QWidget(Constructor);
        centralwidget->setObjectName("centralwidget");
        m_toFolderView = new QTreeView(centralwidget);
        m_toFolderView->setObjectName("m_toFolderView");
        m_toFolderView->setGeometry(QRect(0, 0, 801, 581));
        Constructor->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Constructor);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 806, 22));
        Constructor->setMenuBar(menubar);
        statusbar = new QStatusBar(Constructor);
        statusbar->setObjectName("statusbar");
        Constructor->setStatusBar(statusbar);

        retranslateUi(Constructor);

        QMetaObject::connectSlotsByName(Constructor);
    } // setupUi

    void retranslateUi(QMainWindow *Constructor)
    {
        Constructor->setWindowTitle(QCoreApplication::translate("Constructor", "Constructor", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Constructor: public Ui_Constructor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONSTRUCTOR_H
