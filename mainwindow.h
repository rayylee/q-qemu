#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "config/global_setting.h"
#include "virtualmachine.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void _new_machine_wizard();
    void _main_setting();
    void _delete_vm(const QString&);
    void _cmd_qmp(QString&);

private slots:
    void recv_delete_vm();

    void recv_refresh_vm();

    void recv_refresh_vm_state(const QString&);

    void recv_qemu_dir(QString, QString);

    void recv_new_machine(const QString&);    

    void on_actionAbout_triggered();

    void on_actionNew1_triggered();

    void on_actionNew2_triggered();

    void on_actionStart_triggered();

    void on_actionSetting1_triggered();

    static void on_actionVMSetting_triggered();

    void on_vmlistWidget_customContextMenuRequested(const QPoint &pos);

    void on_actionShutdown_triggered();

    void on_actionPoweroff_triggered();    

    void on_vmlistWidget_currentRowChanged(int currentRow);

    static Q_NORETURN void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;

    QString m_app_dir;
    GlobalSetting *m_global_setting;
    BitMap *m_bitmap;
    QMap<QString, VirtualMachine*> m_vm_map;
};
#endif // MAIN_WINDOW_H
