#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QProcess>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAcceptDrops(true);

    // Connect buttons
    connect(ui->selectXllButton, &QPushButton::clicked, this, &MainWindow::on_selectXllButton_clicked);
    connect(ui->generateButton, &QPushButton::clicked, this, &MainWindow::on_generateButton_clicked);

    ui->generateButton->setEnabled(false);
    ui->statusLabel->setText("Waiting for input...");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        auto files = mimeData->urls();
        if (!files.isEmpty()) {
            payloadFile = files[0].toLocalFile();
            ui->payloadLabel->setText("Payload: " + QFileInfo(payloadFile).fileName());
            checkReady();
        }
    }
}

void MainWindow::on_selectXllButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Select XLL File", "", "XLL Files (*.xll);;All Files (*)");
    if (!file.isEmpty()) {
        xllFile = file;
        ui->selectXllButton->setText("XLL: " + QFileInfo(xllFile).fileName());
        checkReady();
    }
}

void MainWindow::on_generateButton_clicked()
{
    QString outputStub = QFileDialog::getSaveFileName(this, "Save Stub As", "", "C++ Files (*.cpp);;All Files (*)");
    if (outputStub.isEmpty()) return;

    // Build command - currently only uses payload, output
    // If you want to use xllFile as an argument, update native_stub_generator to accept a third file
    QString cmd = QString("./native_stub_generator \"%1\" \"%2\"").arg(payloadFile, outputStub);
    QProcess process;
    process.start(cmd);
    process.waitForFinished();
    int ret = process.exitCode();
    if (ret == 0) {
        QMessageBox::information(this, "Done", "Stub generated! Now compile the stub in C++ (VS or g++).");
    } else {
        QMessageBox::critical(this, "Error", "Failed to generate stub. Check input files and try again.");
    }
}

void MainWindow::checkReady()
{
    ui->generateButton->setEnabled(!payloadFile.isEmpty() && !xllFile.isEmpty());
}
