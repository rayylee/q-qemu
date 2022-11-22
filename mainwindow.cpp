#include <QDir>
#include <QStandardPaths>
#include <QLabel>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helpdialog.h"
#include "newmachinewidget.h"
#include "mainsetting.h"
#include "virtualmachine.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->toolBar->layout()->setSpacing(20);

    ui->vmlistWidget->addItem("testvm1");
    ui->vmlistWidget->addItem("testvm2");
    ui->vmlistWidget->addItem("testvm3");

    QString home_path = QStandardPaths::writableLocation(
                QStandardPaths::HomeLocation);

    QDir dir_home(home_path);
    m_app_path = QDir::toNativeSeparators(dir_home.absoluteFilePath(".q-qemu"));
    QDir dir_app(m_app_path);
    if(!dir_app.exists()) {
        dir_app.mkpath(m_app_path);
    }

    if (ui->vmlistWidget->count() > 0) {
        ui->vmlistWidget->setCurrentRow(0);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::_new_machine_wizard()
{
    NewMachineWidget *w = new NewMachineWidget();

    w->setWindowModality(Qt::ApplicationModal);   

    w->show();
}

void MainWindow::recv_qemu_dir(QString qemu_dir)
{
    m_qemu_dir = qemu_dir;
}

void MainWindow::on_actionAbout_triggered()
{
    HelpDialog *dialog = new HelpDialog();

    dialog->setWindowModality(Qt::ApplicationModal);

    dialog->show();
}

void MainWindow::on_actionNew1_triggered()
{
    _new_machine_wizard();
}


void MainWindow::on_actionNew2_triggered()
{
    _new_machine_wizard();
}


void MainWindow::on_actionStart_triggered()
{
    QList<QListWidgetItem*> items = ui->vmlistWidget->selectedItems();
    if (items.isEmpty()) {
        return;
    }

    QDir dir_app(m_app_path);
    QString xml_path = QDir::toNativeSeparators(
                dir_app.absoluteFilePath(items.at(0)->text() + ".xml"));

    QFileInfo file_info(xml_path);
    if(!file_info.isFile()) {
         return;
    }

    VirtualMachine vm = VirtualMachine(xml_path);

}

void MainWindow::_main_setting()
{
    MainSetting *w = new MainSetting();

    w->set_qemu_dir(m_qemu_dir);
    w->setWindowModality(Qt::ApplicationModal);

    connect(w, &MainSetting::send_qemu_dir,
            this, &MainWindow::recv_qemu_dir);

    w->show();
}

void MainWindow::on_actionSetting1_triggered()
{
    _main_setting();
}

void MainWindow::on_actionVMSetting_triggered()
{
    qDebug() << "VM Setting";
}

