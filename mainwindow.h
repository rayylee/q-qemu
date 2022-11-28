#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "config/global_setting.h"

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
    void _delete_vm(QString);
    QString _get_curvm_xml_path();
    void _cmd_qmp(QString&);

private slots:
    void recv_delete_vm();

    void recv_refresh_vm();

    void recv_qemu_dir(QString, QString);

    void recv_new_machine(QString);

    void on_actionAbout_triggered();

    void on_actionNew1_triggered();

    void on_actionNew2_triggered();

    void on_actionStart_triggered();

    void on_actionSetting1_triggered();

    void on_actionVMSetting_triggered();

    void on_vmlistWidget_customContextMenuRequested(const QPoint &pos);

    void on_actionShutdown_triggered();

    void on_actionPoweroff_triggered();

private:
    Ui::MainWindow *ui;

    QString m_app_dir;
    QString m_qemu_dir;
    QString m_qemu_binary_path;
    BitMap *m_bitmap;
    QString m_monitor_port;
    QString m_ssh_port;
};
#endif // MAINWINDOW_H
