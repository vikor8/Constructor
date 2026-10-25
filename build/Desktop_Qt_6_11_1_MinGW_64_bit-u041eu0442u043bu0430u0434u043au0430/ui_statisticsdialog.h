/********************************************************************************
** Form generated from reading UI file 'statisticsdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STATISTICSDIALOG_H
#define UI_STATISTICSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_StatisticsDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QGroupBox *filterGroup;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QComboBox *periodCombo;
    QDateEdit *dateFromEdit;
    QLabel *dateSepLabel;
    QDateEdit *dateToEdit;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *groupGrouping;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QRadioButton *radioByOrder;
    QRadioButton *radioByDate;
    QRadioButton *radioByNone;
    QHBoxLayout *horizontalLayout_3;
    QLabel *searchLabel;
    QLineEdit *searchEdit;
    QCheckBox *onlyScoredCheck;
    QSpacerItem *horizontalSpacer;
    QTreeView *statsTree;
    QGroupBox *summaryGroup;
    QVBoxLayout *verticalLayout_2;
    QGridLayout *gridLayout;
    QLabel *labelPeriodTitle;
    QLabel *labelItemsTitle;
    QSpacerItem *horizontalSpacer_4;
    QLabel *labelPointsValue;
    QLabel *labelOrdersTitle;
    QLabel *labelPeriodValue;
    QLabel *labelPointsTitle;
    QSpacerItem *horizontalSpacer_5;
    QSpacerItem *horizontalSpacer_6;
    QLabel *labelOrdersValue;
    QLabel *labelItemsValue;
    QSpacerItem *horizontalSpacer_7;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *exportCsvButton;
    QPushButton *copyButton;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *refreshDataButton;
    QPushButton *closeButton;

    void setupUi(QDialog *StatisticsDialog)
    {
        if (StatisticsDialog->objectName().isEmpty())
            StatisticsDialog->setObjectName("StatisticsDialog");
        StatisticsDialog->resize(900, 600);
        StatisticsDialog->setMinimumSize(QSize(900, 600));
        StatisticsDialog->setSizeGripEnabled(true);
        verticalLayout_3 = new QVBoxLayout(StatisticsDialog);
        verticalLayout_3->setObjectName("verticalLayout_3");
        filterGroup = new QGroupBox(StatisticsDialog);
        filterGroup->setObjectName("filterGroup");
        filterGroup->setMinimumSize(QSize(600, 120));
        verticalLayout = new QVBoxLayout(filterGroup);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        periodCombo = new QComboBox(filterGroup);
        periodCombo->setObjectName("periodCombo");
        periodCombo->setMinimumSize(QSize(300, 0));

        horizontalLayout->addWidget(periodCombo);

        dateFromEdit = new QDateEdit(filterGroup);
        dateFromEdit->setObjectName("dateFromEdit");

        horizontalLayout->addWidget(dateFromEdit);

        dateSepLabel = new QLabel(filterGroup);
        dateSepLabel->setObjectName("dateSepLabel");
        dateSepLabel->setMinimumSize(QSize(10, 22));

        horizontalLayout->addWidget(dateSepLabel);

        dateToEdit = new QDateEdit(filterGroup);
        dateToEdit->setObjectName("dateToEdit");

        horizontalLayout->addWidget(dateToEdit);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout);

        groupGrouping = new QGroupBox(filterGroup);
        groupGrouping->setObjectName("groupGrouping");
        groupGrouping->setMinimumSize(QSize(400, 40));
        widget = new QWidget(groupGrouping);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(0, 15, 374, 24));
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        radioByOrder = new QRadioButton(widget);
        radioByOrder->setObjectName("radioByOrder");
        radioByOrder->setMinimumSize(QSize(120, 22));
        radioByOrder->setChecked(true);

        horizontalLayout_2->addWidget(radioByOrder);

        radioByDate = new QRadioButton(widget);
        radioByDate->setObjectName("radioByDate");
        radioByDate->setMinimumSize(QSize(120, 22));

        horizontalLayout_2->addWidget(radioByDate);

        radioByNone = new QRadioButton(widget);
        radioByNone->setObjectName("radioByNone");
        radioByNone->setMinimumSize(QSize(120, 22));

        horizontalLayout_2->addWidget(radioByNone);


        verticalLayout->addWidget(groupGrouping);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        searchLabel = new QLabel(filterGroup);
        searchLabel->setObjectName("searchLabel");
        searchLabel->setMinimumSize(QSize(90, 22));

        horizontalLayout_3->addWidget(searchLabel);

        searchEdit = new QLineEdit(filterGroup);
        searchEdit->setObjectName("searchEdit");
        searchEdit->setMinimumSize(QSize(150, 22));

        horizontalLayout_3->addWidget(searchEdit);

        onlyScoredCheck = new QCheckBox(filterGroup);
        onlyScoredCheck->setObjectName("onlyScoredCheck");
        onlyScoredCheck->setMinimumSize(QSize(120, 22));
        onlyScoredCheck->setChecked(true);

        horizontalLayout_3->addWidget(onlyScoredCheck);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_3);


        verticalLayout_3->addWidget(filterGroup);

        statsTree = new QTreeView(StatisticsDialog);
        statsTree->setObjectName("statsTree");
        statsTree->setStyleSheet(QString::fromUtf8("QTreeView {\n"
"    gridline-color: #d0d0d0;\n"
"    show-decoration-selected: 1;\n"
"}\n"
"\n"
"QTreeView::item {\n"
"    border-right: 1px solid #d0d0d0;\n"
"    border-bottom: 1px solid #d0d0d0;\n"
"    padding: 4px 6px;\n"
"}"));
        statsTree->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
        statsTree->setAlternatingRowColors(true);
        statsTree->setUniformRowHeights(true);
        statsTree->setSortingEnabled(true);
        statsTree->setHeaderHidden(true);

        verticalLayout_3->addWidget(statsTree);

        summaryGroup = new QGroupBox(StatisticsDialog);
        summaryGroup->setObjectName("summaryGroup");
        verticalLayout_2 = new QVBoxLayout(summaryGroup);
        verticalLayout_2->setObjectName("verticalLayout_2");
        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        gridLayout->setHorizontalSpacing(20);
        labelPeriodTitle = new QLabel(summaryGroup);
        labelPeriodTitle->setObjectName("labelPeriodTitle");

        gridLayout->addWidget(labelPeriodTitle, 0, 9, 1, 1);

        labelItemsTitle = new QLabel(summaryGroup);
        labelItemsTitle->setObjectName("labelItemsTitle");

        gridLayout->addWidget(labelItemsTitle, 0, 3, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_4, 0, 2, 1, 1);

        labelPointsValue = new QLabel(summaryGroup);
        labelPointsValue->setObjectName("labelPointsValue");

        gridLayout->addWidget(labelPointsValue, 0, 7, 1, 1);

        labelOrdersTitle = new QLabel(summaryGroup);
        labelOrdersTitle->setObjectName("labelOrdersTitle");

        gridLayout->addWidget(labelOrdersTitle, 0, 0, 1, 1);

        labelPeriodValue = new QLabel(summaryGroup);
        labelPeriodValue->setObjectName("labelPeriodValue");

        gridLayout->addWidget(labelPeriodValue, 0, 10, 1, 1);

        labelPointsTitle = new QLabel(summaryGroup);
        labelPointsTitle->setObjectName("labelPointsTitle");

        gridLayout->addWidget(labelPointsTitle, 0, 6, 1, 1);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_5, 0, 5, 1, 1);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_6, 0, 8, 1, 1);

        labelOrdersValue = new QLabel(summaryGroup);
        labelOrdersValue->setObjectName("labelOrdersValue");

        gridLayout->addWidget(labelOrdersValue, 0, 1, 1, 1);

        labelItemsValue = new QLabel(summaryGroup);
        labelItemsValue->setObjectName("labelItemsValue");

        gridLayout->addWidget(labelItemsValue, 0, 4, 1, 1);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_7, 0, 11, 1, 1);


        verticalLayout_2->addLayout(gridLayout);


        verticalLayout_3->addWidget(summaryGroup);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        exportCsvButton = new QPushButton(StatisticsDialog);
        exportCsvButton->setObjectName("exportCsvButton");
        exportCsvButton->setMinimumSize(QSize(90, 24));

        horizontalLayout_4->addWidget(exportCsvButton);

        copyButton = new QPushButton(StatisticsDialog);
        copyButton->setObjectName("copyButton");
        copyButton->setMinimumSize(QSize(75, 24));

        horizontalLayout_4->addWidget(copyButton);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);

        refreshDataButton = new QPushButton(StatisticsDialog);
        refreshDataButton->setObjectName("refreshDataButton");
        refreshDataButton->setMinimumSize(QSize(75, 24));

        horizontalLayout_4->addWidget(refreshDataButton);

        closeButton = new QPushButton(StatisticsDialog);
        closeButton->setObjectName("closeButton");
        closeButton->setMinimumSize(QSize(75, 24));

        horizontalLayout_4->addWidget(closeButton);


        verticalLayout_3->addLayout(horizontalLayout_4);


        retranslateUi(StatisticsDialog);

        QMetaObject::connectSlotsByName(StatisticsDialog);
    } // setupUi

    void retranslateUi(QDialog *StatisticsDialog)
    {
        StatisticsDialog->setWindowTitle(QCoreApplication::translate("StatisticsDialog", "\320\241\321\202\320\260\321\202\320\270\321\201\321\202\320\270\320\272\320\260 \321\200\320\260\320\261\320\276\321\202", nullptr));
        filterGroup->setTitle(QCoreApplication::translate("StatisticsDialog", "\320\237\320\265\321\200\320\270\320\276\320\264", nullptr));
        dateSepLabel->setText(QCoreApplication::translate("StatisticsDialog", "\342\200\224", nullptr));
        groupGrouping->setTitle(QCoreApplication::translate("StatisticsDialog", "\320\223\321\200\321\203\320\277\320\277\320\270\321\200\320\276\320\262\320\272\320\260", nullptr));
        radioByOrder->setText(QCoreApplication::translate("StatisticsDialog", "\320\237\320\276 \320\267\320\260\320\272\320\260\320\267\321\203", nullptr));
        radioByDate->setText(QCoreApplication::translate("StatisticsDialog", "\320\237\320\276 \320\264\320\260\321\202\320\265", nullptr));
        radioByNone->setText(QCoreApplication::translate("StatisticsDialog", "\320\221\320\265\320\267 \320\263\321\200\321\203\320\277\320\277\320\270\321\200\320\276\320\262\320\272\320\270", nullptr));
        searchLabel->setText(QCoreApplication::translate("StatisticsDialog", "\320\237\320\276\320\270\321\201\320\272 \320\270\320\267\320\264\320\265\320\273\320\270\321\217:", nullptr));
        onlyScoredCheck->setText(QCoreApplication::translate("StatisticsDialog", "\320\242\320\276\320\273\321\214\320\272\320\276 \321\201 \320\261\320\260\320\273\320\273\320\260\320\274\320\270", nullptr));
        summaryGroup->setTitle(QCoreApplication::translate("StatisticsDialog", "\320\241\320\262\320\276\320\264\320\272\320\260", nullptr));
        labelPeriodTitle->setText(QCoreApplication::translate("StatisticsDialog", "\320\237\320\265\321\200\320\270\320\276\320\264:", nullptr));
        labelItemsTitle->setText(QCoreApplication::translate("StatisticsDialog", "\320\222\321\201\320\265\320\263\320\276 \320\270\320\267\320\264\320\265\320\273\320\270\320\271:", nullptr));
        labelPointsValue->setText(QCoreApplication::translate("StatisticsDialog", "0", nullptr));
        labelOrdersTitle->setText(QCoreApplication::translate("StatisticsDialog", "\320\222\321\201\320\265\320\263\320\276 \320\267\320\260\320\272\320\260\320\267\320\276\320\262:", nullptr));
        labelPeriodValue->setText(QCoreApplication::translate("StatisticsDialog", "\342\200\224", nullptr));
        labelPointsTitle->setText(QCoreApplication::translate("StatisticsDialog", "\320\222\321\201\320\265\320\263\320\276 \320\261\320\260\320\273\320\273\320\276\320\262:", nullptr));
        labelOrdersValue->setText(QCoreApplication::translate("StatisticsDialog", "0", nullptr));
        labelItemsValue->setText(QCoreApplication::translate("StatisticsDialog", "0", nullptr));
        exportCsvButton->setText(QCoreApplication::translate("StatisticsDialog", "\320\255\320\272\321\201\320\277\320\276\321\200\321\202 \320\262 CSV", nullptr));
        copyButton->setText(QCoreApplication::translate("StatisticsDialog", "\320\232\320\276\320\277\320\270\321\200\320\276\320\262\320\260\321\202\321\214", nullptr));
        refreshDataButton->setText(QCoreApplication::translate("StatisticsDialog", "\320\236\320\261\320\275\320\276\320\262\320\270\321\202\321\214", nullptr));
        closeButton->setText(QCoreApplication::translate("StatisticsDialog", "\320\227\320\260\320\272\321\200\321\213\321\202\321\214", nullptr));
    } // retranslateUi

};

namespace Ui {
    class StatisticsDialog: public Ui_StatisticsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STATISTICSDIALOG_H
