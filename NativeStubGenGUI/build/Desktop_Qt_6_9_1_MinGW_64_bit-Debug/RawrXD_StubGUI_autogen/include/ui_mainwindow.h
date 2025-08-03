/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QLabel *payloadLabel;
    QLabel *statusLabel;
    QPushButton *selectXllButton;
    QPushButton *generateButton;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        payloadLabel = new QLabel(centralwidget);
        payloadLabel->setObjectName("payloadLabel");

        verticalLayout->addWidget(payloadLabel);

        statusLabel = new QLabel(centralwidget);
        statusLabel->setObjectName("statusLabel");

        verticalLayout->addWidget(statusLabel);

        selectXllButton = new QPushButton(centralwidget);
        selectXllButton->setObjectName("selectXllButton");

        verticalLayout->addWidget(selectXllButton);

        generateButton = new QPushButton(centralwidget);
        generateButton->setObjectName("generateButton");

        verticalLayout->addWidget(generateButton);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        payloadLabel->setText(QCoreApplication::translate("MainWindow", "Drag your payload file here\n"
"", nullptr));
        statusLabel->setText(QCoreApplication::translate("MainWindow", "\342\200\234Waiting for input...\342\200\235", nullptr));
        selectXllButton->setText(QCoreApplication::translate("MainWindow", "Select XLL File", nullptr));
        generateButton->setText(QCoreApplication::translate("MainWindow", "Generate Stub", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
