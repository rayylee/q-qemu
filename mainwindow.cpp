#include <QDir>
#include <QStandardPaths>
#include <QLabel>
#include <QDebug>
#include <QDirIterator>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helpdialog.h"
#include "newmachinewidget.h"
#include "mainsetting.h"
#include "virtualmachine.h"
#include "virtualmachinedom.h"
#include "config/global_setting.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->toolBar->layout()->setSpacing(20);

    QString home_path = QStandardPaths::writableLocation(
                QStandardPaths::HomeLocation);
    QDir dir_home(home_path);
    m_app_dir = QDir::toNativeSeparators(dir_home.absoluteFilePath(".q-qemu"));
    QDir dir_app(m_app_dir);
    if(!dir_app.exists()) {
        dir_app.mkpath(m_app_dir);
    }

    QDirIterator it(m_app_dir, QStringList() << "*.xml",
                    QDir::NoFilter, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        QString file_name = it.fileName();
        if (file_name.endsWith(".xml")) {
            QString vm_item = file_name.left(file_name.length() - 4);
            ui->vmlistWidget->addItem(vm_item);
        }
    }

    if (ui->vmlistWidget->count() > 0) {
        ui->vmlistWidget->setCurrentRow(0);
    }

    GlobalSetting s = GlobalSetting(m_app_dir);
    m_qemu_dir = s.qemu_dir();
    if (m_qemu_dir.isEmpty()) {
        m_qemu_dir = QDir::toNativeSeparators("D:/Program Files/qemu");
        s.set_qemu_dir(m_qemu_dir);
        s.search_qemu_binary();
        s.save_config();
    }
    m_qemu_binary_path = s.qemu_binary_path();
    m_bitmap = new BitMap(s.bitmap_string());
    m_ssh_port = s.ssh_port();
    m_monitor_port = s.monitor_port();
}

MainWindow::~MainWindow()
{
    GlobalSetting s = GlobalSetting(m_app_dir);
    QString bitmap_string = m_bitmap->to_string();
    bool config_chaned = false;
    if (s.bitmap_string().compare(bitmap_string)) {
        s.set_bitmap_string(bitmap_string);        
        config_chaned = true;
    }    
    if (s.ssh_port().compare(m_ssh_port)) {
        s.set_ssh_port(m_ssh_port);
        config_chaned = true;
    }
    if (s.monitor_port().compare(m_monitor_port)) {
        s.set_monitor_port(m_monitor_port);
        config_chaned = true;
    }
    if (config_chaned) {
        s.save_config();
    }


    delete m_bitmap;
    delete ui;
}

void MainWindow::_new_machine_wizard()
{
    NewMachineWidget *w = new NewMachineWidget();

    w->setWindowModality(Qt::ApplicationModal);

    int domin_id = m_bitmap->get_first_zero() + 1;
    qDebug() << "New domain id:" << domin_id;
    if (domin_id > 0) {
        w->set_domain_id(domin_id);
    }
    w->set_app_dir(m_app_dir);
    w->set_qemu_binary_path(m_qemu_binary_path);

    connect(w, &NewMachineWidget::send_new_machine,
            this, &MainWindow::recv_new_machine);

    w->exec();
    delete w;
}

void MainWindow::recv_new_machine(QString vm_name)
{
    ui->vmlistWidget->addItem(vm_name);
    int domin_id = m_bitmap->get_first_zero() + 1;
    m_bitmap->set(domin_id - 1);
}

void MainWindow::recv_qemu_dir(QString qemu_dir, QString qemu_binary_path)
{
    m_qemu_dir = qemu_dir;
    m_qemu_binary_path = qemu_binary_path;
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

QString MainWindow::_get_curvm_xml_path()
{
    QList<QListWidgetItem*> items = ui->vmlistWidget->selectedItems();
    if (items.isEmpty()) {
        return QString("");
    }

    QDir dir_app(m_app_dir);
    QString xml_path = QDir::toNativeSeparators(
                dir_app.absoluteFilePath(items.at(0)->text() + ".xml"));

    QFileInfo file_info(xml_path);
    if(!file_info.isFile()) {
         return QString("");
    }
    return xml_path;
}

void MainWindow::_main_setting()
{
    MainSetting *w = new MainSetting();

    w->set_app_dir(m_app_dir);
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

void MainWindow::on_vmlistWidget_customContextMenuRequested(const QPoint &pos)
{
    QListWidgetItem* curItem = ui->vmlistWidget->itemAt(pos);
    if(!curItem) {
        QMenu *blank_menu = new QMenu(this);
        QAction *refresh_action = new QAction(tr("Refresh"), this);

        blank_menu->addAction(refresh_action);

        connect(refresh_action, &QAction::triggered,
                this, &MainWindow::recv_refresh_vm);

        blank_menu->exec(QCursor::pos());

        delete blank_menu;
        delete refresh_action;
        return;
    }

    QMenu *item_menu = new QMenu(this);
    QAction *delete_vm = new QAction(tr("Delete"), this);

    item_menu->addAction(delete_vm);

    connect(delete_vm, &QAction::triggered, this, &MainWindow::recv_delete_vm);

    item_menu->exec(QCursor::pos());

    delete item_menu;
    delete delete_vm;
}

void MainWindow::recv_refresh_vm()
{
    QStringList cur_names;
    for (int i = 0; i < ui->vmlistWidget->count(); i++) {
        QListWidgetItem *item = ui->vmlistWidget->item(i);
        cur_names.append(item->text());
    }

    QStringList file_names;
    QDirIterator it(m_app_dir, QStringList() << "*.xml",
                    QDir::NoFilter, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        QString file_name = it.fileName();
        if (file_name.endsWith(".xml")) {
            QString vm_item = file_name.left(file_name.length() - 4);
            file_names.append(vm_item);
        }
    }

    for (int i = 0; i < file_names.count(); i++) {
        if (!cur_names.contains(file_names.at(i))) {
            cur_names.append(file_names.at(i));
            ui->vmlistWidget->addItem(file_names.at(i));
        }
    }

    QStringList remove_names;
    for (int i = 0; i < cur_names.count(); i++) {
        if (!file_names.contains(cur_names.at(i))) {
            remove_names.append(cur_names.at(i));
        }
    }

    for (int i = 0; i < remove_names.count(); i++) {
        _delete_vm(remove_names.at(i));
    }
}

void MainWindow::_delete_vm(QString vm_name)
{
    QListWidgetItem *item = nullptr;

    for (int i = 0; i < ui->vmlistWidget->count(); i++) {
        item = ui->vmlistWidget->item(i);
        if (!item->text().compare(vm_name)) {
            break;
        }
        item = nullptr;
    }

    if (!item) {
        return;
    }

    QDir dir_app(m_app_dir);
    QString xml_path = QDir::toNativeSeparators(
                dir_app.absoluteFilePath(vm_name + ".xml"));

    QFile f(xml_path);
    bool should_remove = false;
    if (f.exists()) {
        if (f.fileName().endsWith("xml")) {
            should_remove = true;            
            VirtualMachineDom vm_dom = VirtualMachineDom(f);
            QString domain_id = vm_dom.get_domain_id();
            qDebug() << "Remove vm:" << vm_name << ",id:" << domain_id;
            if (domain_id.length() > 0) {
                m_bitmap->clear(domain_id.toInt() - 1);
            }

            f.remove();
        }
    } else {
        should_remove = true;
    }

    if (should_remove) {
        int cur_index = ui->vmlistWidget->row(item);
        ui->vmlistWidget->takeItem(cur_index);        
        delete item;
    }
}

void MainWindow::recv_delete_vm()
{
    int ch = QMessageBox::warning(this, "Warning",
                                  "Are you sure to delete?",
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::No);

    if (ch != QMessageBox::Yes) {
        return;
    }

    QListWidgetItem *item = ui->vmlistWidget->currentItem();
    if (!item) {
        return;
    }
    QString vm_name = item->text();
    _delete_vm(vm_name);
}

void MainWindow::on_actionStart_triggered()
{
    QString xml_path = _get_curvm_xml_path();

    if (xml_path.length() <= 0) {
        return;
    }

    VirtualMachine vm = VirtualMachine(xml_path);

    vm.start(m_ssh_port, m_monitor_port);
}

void MainWindow::_cmd_qmp(QString& cmd)
{
    QString xml_path = _get_curvm_xml_path();
    if (xml_path.length() <= 0) {
        return;
    }

    VirtualMachine vm = VirtualMachine(xml_path);

    uint16_t monitor_listen = vm.domain_id().toUShort()
             + m_monitor_port.toUShort() - 1;

    QTcpSocket *monitor_socket = new QTcpSocket();
    monitor_socket->connectToHost("localhost",
                                    monitor_listen,
                                    QIODevice::ReadWrite);

    monitor_socket->waitForConnected(100);

    if (monitor_socket->state() == QAbstractSocket::ConnectedState) {
        monitor_socket->write(qPrintable(QString("%1\n").arg(cmd)));
    } else {
        qCritical() << "Error: connect"
                    << monitor_listen
                    << monitor_socket->state();
    }
}

void MainWindow::on_actionShutdown_triggered()
{
    QString cmd = QString("system_powerdown");
    _cmd_qmp(cmd);
}

void MainWindow::on_actionPoweroff_triggered()
{
    QString cmd = QString("quit");
    _cmd_qmp(cmd);
}
