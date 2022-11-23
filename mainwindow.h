#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void _new_machine_wizard();
    void _main_setting();

private slots:
    void recv_qemu_dir(QString, QString);

    void on_actionAbout_triggered();

    void on_actionNew1_triggered();

    void on_actionNew2_triggered();

    void on_actionStart_triggered();

    void on_actionSetting1_triggered();

    void on_actionVMSetting_triggered();

private:
    Ui::MainWindow *ui;

    QString m_app_dir;
    QString m_qemu_dir;
    QString m_qemu_binary_path;
};
#endif // MAINWINDOW_H
