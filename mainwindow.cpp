#include <QDir>
#include <QStandardPaths>
#include <QLabel>
#include <QDebug>
#include <QWindow>
#include <QProcess>
#include <QDirIterator>
#include <QMessageBox>
#include <utility>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helpdialog.h"
#include "newmachinewidget.h"
#include "mainsetting.h"
#include "virtualmachine.h"
#include "virtualmachinedom.h"

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

            auto *vm = new VirtualMachine(it.filePath());

            connect(vm, &VirtualMachine::send_vm_state_changed,
                    this, &MainWindow::recv_refresh_vm_state);

            m_vm_map.insert(vm_item, vm);
        }
    }

    if (ui->vmlistWidget->count() > 0) {
        ui->vmlistWidget->setCurrentRow(0);
        for (int i = 0; i < ui->vmlistWidget->count(); i++) {
            auto item = ui->vmlistWidget->item(i);
            item->setIcon(QIcon(QPixmap(tr(":/Resources/shutdown_flag.png"))));
        }
    }

    m_global_setting = new GlobalSetting(m_app_dir);
    m_bitmap = new BitMap(m_global_setting->bitmap_string());
}

MainWindow::~MainWindow()
{
    qDebug() << "quit";

    QString bitmap_string = m_bitmap->to_string();
    if (m_global_setting->bitmap_string().compare(bitmap_string)) {
        m_global_setting->set_bitmap_string(bitmap_string);
        m_global_setting->save_config();
    }

    delete m_bitmap;
    delete m_global_setting;
    delete ui;
}

void MainWindow::_new_machine_wizard()
{
    auto *w = new NewMachineWidget();

    w->setWindowModality(Qt::ApplicationModal);

    int domain_id = m_bitmap->get_first_zero() + 1;
    qDebug() << "New domain id:" << domain_id;
    if (domain_id > 0) {
        w->set_domain_id(domain_id);
    }
    w->set_app_dir(m_app_dir);
    w->set_qemu_binary_path(m_global_setting->qemu_binary_path());

    connect(w, &NewMachineWidget::send_new_machine,
            this, &MainWindow::recv_new_machine);

    w->exec();
    delete w;
}

void MainWindow::recv_new_machine(const QString& vm_name)
{
    ui->vmlistWidget->addItem(vm_name);
    int domain_id = m_bitmap->get_first_zero() + 1;
    m_bitmap->set(domain_id - 1);

    QDir dir_app(m_app_dir);
    QString xml_path = QDir::toNativeSeparators(
            dir_app.absoluteFilePath(vm_name + ".xml"));

    m_vm_map.insert(vm_name, new VirtualMachine(xml_path));
}

void MainWindow::recv_qemu_dir(QString qemu_dir, QString qemu_binary_path)
{
    m_global_setting->set_qemu_dir(std::move(qemu_dir));
    m_global_setting->set_qemu_binary(std::move(qemu_binary_path));
    m_global_setting->save_config();
}

void MainWindow::on_actionAbout_triggered()
{
    auto *dialog = new HelpDialog();

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

void MainWindow::_main_setting()
{
    auto *w = new MainSetting();

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
        auto *blank_menu = new QMenu(this);
        auto *refresh_action = new QAction(tr("Refresh"), this);

        blank_menu->addAction(refresh_action);

        connect(refresh_action, &QAction::triggered,
                this, &MainWindow::recv_refresh_vm);

        blank_menu->exec(QCursor::pos());

        delete blank_menu;
        delete refresh_action;
        return;
    }

    auto *item_menu = new QMenu(this);
    auto *delete_vm = new QAction(tr("Delete"), this);

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

void MainWindow::_delete_vm(const QString& vm_name)
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
        if (m_vm_map.contains(vm_name)) {
            m_vm_map.value(vm_name)->stop();
            m_vm_map.remove(vm_name);
        }
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
    QList<QListWidgetItem*> items = ui->vmlistWidget->selectedItems();
    if (items.isEmpty()) {
        return;
    }

    auto vm = m_vm_map.value((items.at(0)->text()));

    vm->start(m_global_setting->ssh_port(),
             m_global_setting->vnc_port());

}

void MainWindow::_cmd_qmp(QString& cmd)
{
    QList<QListWidgetItem*> items = ui->vmlistWidget->selectedItems();
    if (items.isEmpty()) {
        return;
    }

    auto vm = m_vm_map.value((items.at(0)->text()));

    const char *data = qPrintable(QString("%1\n").arg(cmd));
    if (vm->process_write(data, static_cast<qint64>(strlen(data))) != strlen(data) ) {
        qCritical("Error: process write");
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

void MainWindow::on_vmlistWidget_currentRowChanged(int currentRow)
{
    auto item = ui->vmlistWidget->item(currentRow);
    auto vm = m_vm_map.value((item->text()));

    ui->vmNameLabel->setText(vm->name());
    ui->vmStateLabel->setText(vm->state().to_string());

    auto ssh_port = vm->ssh_listen_port();
    if (ssh_port > 0) {
        ui->vmSshPortLabel->setText(QString("%1").
                                    arg(vm->ssh_listen_port()));
    } else {
        ui->vmSshPortLabel->setText("-");
    }

    auto vnc_port = vm->vnc_listen_port();
    if (vnc_port > 0) {
        ui->vmVncPortLabel->setText(QString("%1").
                                    arg(vm->vnc_listen_port()));
    } else {
        ui->vmVncPortLabel->setText("-");
    }

    ui->vmAcceleratorLabel->setText(vm->accelerator());
    ui->vmCpuLabel->setText(
                QString("Sockets:%1  Cores:%2  Threads:%3")
                .arg(vm->cpu_sockets())
                .arg(vm->cpu_cores())
                .arg(vm->cpu_threads()));
    ui->vmCurrentMemoryLabel->setText(vm->current_memory());

    int max_length_disk = 400;
    QString path = vm->disk(0);
    if (path.length() > max_length_disk) {
        ui->vmHD1LineEdit->setText("..." + path.right(max_length_disk));
    } else {
        if (path.length() > 0) {
            ui->vmHD1LineEdit->setText(path);
        } else {
            ui->vmHD1LineEdit->clear();
        }
    }
    ui->vmHD1LineEdit->setToolTip(path);
    ui->vmHD1LineEdit->setCursorPosition(0);

    path = vm->disk(1);
    if (path.length() > max_length_disk) {
        ui->vmHD2LineEdit->setText("..." + path.right(max_length_disk));
    } else {
        if (path.length() > 0) {
            ui->vmHD2LineEdit->setText(path);
        } else {
            ui->vmHD2LineEdit->clear();
        }
    }
    ui->vmHD2LineEdit->setToolTip(path);
    ui->vmHD2LineEdit->setCursorPosition(0);

    path = vm->cdrom(0);
    if (path.length() > max_length_disk) {
        ui->vmCDRomLineEdit->setText("..." + path.right(max_length_disk));
    } else {
        if (path.length() > 0) {
            ui->vmCDRomLineEdit->setText(path);
        } else {
            ui->vmCDRomLineEdit->clear();
        }
    }
    ui->vmCDRomLineEdit->setToolTip(path);
    ui->vmCDRomLineEdit->setCursorPosition(0);
}

void MainWindow::recv_refresh_vm_state(const QString& name)
{
    QListWidgetItem *item = ui->vmlistWidget->currentItem();
    if (!item) {
        return;
    }

    auto vm = m_vm_map.value((item->text()));

    if (!vm->name().compare(name)) {
        if (vm->state().value() == VMState::StateRunning) {
            item->setIcon(QIcon(QPixmap(tr(":/Resources/start_flag.png"))));
        } else {
            item->setIcon(QIcon(QPixmap(tr(":/Resources/shutdown_flag.png"))));
        }

        ui->vmStateLabel->setText(vm->state().to_string());        

        auto ssh_port = vm->ssh_listen_port();
        if (ssh_port > 0) {
            ui->vmSshPortLabel->setText(QString("%1").
                                        arg(vm->ssh_listen_port()));
        } else {
            ui->vmSshPortLabel->setText("-");
        }

        auto vnc_port = vm->vnc_listen_port();
        if (vnc_port > 0) {
            ui->vmVncPortLabel->setText(QString("%1").
                                        arg(vm->vnc_listen_port()));
        } else {
            ui->vmVncPortLabel->setText("-");
        }
    }
}

void MainWindow::on_actionExit_triggered()
{
    exit(0);
}
