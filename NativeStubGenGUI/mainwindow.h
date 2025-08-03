#pragma once

#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void on_generateButton_clicked();
    void on_polymorphicCheckBox_toggled(bool checked);
    void on_obfuscationCheckBox_toggled(bool checked);
    void on_antiDebugCheckBox_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QString payloadFile;
    bool usePolymorphism;
    bool useObfuscation;
    bool useAntiDebug;
    void checkReady();
};
