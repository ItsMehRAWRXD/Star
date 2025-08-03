#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QTimer>
#include <QFont>
#include <QStyle>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setSpacing(20);
    layout->setContentsMargins(40, 40, 40, 40);
    
    // Title
    titleLabel = new QLabel("Star-2 Drag & Drop Encryptor", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #2c3e50;");
    layout->addWidget(titleLabel);
    
    // Drop area
    dropLabel = new QLabel("Drag and drop files here to encrypt them", this);
    dropLabel->setAlignment(Qt::AlignCenter);
    dropLabel->setMinimumSize(400, 200);
    dropLabel->setStyleSheet(
        "QLabel {"
        "   border: 3px dashed #bdc3c7;"
        "   border-radius: 10px;"
        "   background-color: #ecf0f1;"
        "   color: #7f8c8d;"
        "   font-size: 14px;"
        "}"
        "QLabel:hover {"
        "   border-color: #3498db;"
        "   background-color: #e8f4fd;"
        "}"
    );
    layout->addWidget(dropLabel);
    
    // Status
    statusLabel = new QLabel("Ready to encrypt files...", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("color: #27ae60; font-size: 12px;");
    layout->addWidget(statusLabel);
    
    // Window setup
    setWindowTitle("Star-2 Drag & Drop Encryptor");
    setMinimumSize(500, 400);
    
    // Center window
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        dropLabel->setStyleSheet(
            "QLabel {"
            "   border: 3px dashed #27ae60;"
            "   border-radius: 10px;"
            "   background-color: #d5f4e6;"
            "   color: #27ae60;"
            "   font-size: 14px;"
            "}"
        );
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        for (const QUrl &url : urlList) {
            QString filePath = url.toLocalFile();
            if (!filePath.isEmpty()) {
                encryptFile(filePath);
            }
        }
    }
    
    // Reset drop area style
    dropLabel->setStyleSheet(
        "QLabel {"
        "   border: 3px dashed #bdc3c7;"
        "   border-radius: 10px;"
        "   background-color: #ecf0f1;"
        "   color: #7f8c8d;"
        "   font-size: 14px;"
        "}"
        "QLabel:hover {"
        "   border-color: #3498db;"
        "   background-color: #e8f4fd;"
        "}"
    );
}

void MainWindow::encryptFile(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        statusLabel->setText("Error: File does not exist");
        statusLabel->setStyleSheet("color: #e74c3c; font-size: 12px;");
        return;
    }
    
    QString outputPath = filePath + ".enc";
    
    // Use the native encryptor
    QProcess process;
    process.start("./native_encryptor", QStringList() << filePath);
    process.waitForFinished();
    
    if (process.exitCode() == 0) {
        statusLabel->setText(QString("✓ Encrypted: %1").arg(fileInfo.fileName()));
        statusLabel->setStyleSheet("color: #27ae60; font-size: 12px;");
        
        // Show success message
        QMessageBox::information(this, "Success", 
            QString("File encrypted successfully!\n\n"
                   "Original: %1\n"
                   "Encrypted: %2").arg(fileInfo.fileName(), fileInfo.fileName() + ".enc"));
    } else {
        statusLabel->setText(QString("✗ Failed to encrypt: %1").arg(fileInfo.fileName()));
        statusLabel->setStyleSheet("color: #e74c3c; font-size: 12px;");
        
        QMessageBox::critical(this, "Error", 
            QString("Failed to encrypt file: %1").arg(fileInfo.fileName()));
    }
}