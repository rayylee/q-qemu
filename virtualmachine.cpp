#include <QDebug>
#include <QFile>
#include <QDir>
#include <QProcess>

#include "virtualmachine.h"
#include "virtualmachinedom.h"

VirtualMachine::VirtualMachine()
= default;

VirtualMachine::VirtualMachine(const QString& xml_path)
{
    m_ssh_listen = 0;
    m_monitor_listen = 0;

    QFile file(xml_path);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QString all_string = QString(file.readAll());
    file.close();    

    VirtualMachineDom dom = VirtualMachineDom(all_string);
    m_domain_id = dom.get_domain_id();
    m_accelerator = dom.get_accelerator();
    m_qemu_binary_path = dom.get_emulator();
    m_name = dom.get_name();
    m_current_memory = dom.get_current_memory();
    m_cpu_sockets = dom.get_cpu_sockets();
    m_cpu_cores = dom.get_cpu_cores();
    m_cpu_threads = dom.get_cpu_threads();
    m_os_type = dom.get_os_type();
    int disk_count = dom.get_disk_count();
    for (int i = 0; i < disk_count && i < MAX_DISK_COUNT ; i++) {
        m_disks[i].driver_type = dom.get_disk_type(i);
        m_disks[i].file_path = dom.get_disk_file(i);
    }
    int cdrom_count = dom.get_cdrom_count();
    for (int i = 0; i < cdrom_count && i < MAX_CDROM_COUNT ; i++) {
        m_cdroms[i].file_path = dom.get_cdrom_file(i);
    }
    QStringList orders = dom.get_boot_order();
    for (int i = 0; i < orders.length() && i < MAX_ORDER_COUNT ; i++) {
        m_orders[i].order = orders.at(i);
    }
}

VirtualMachine::~VirtualMachine()
= default;

void VirtualMachine::start(const QString& ssh_port, const QString& monitor_port)
{
    QStringList qemuCommand;

    m_ssh_listen = ssh_port.toUInt() + m_domain_id.toUInt() - 1;
    m_monitor_listen = monitor_port.toUInt() + m_domain_id.toUInt() - 1;

    // a,b: floppy disk
    // c  : hard disk
    // d  : cdrom
    // n-p: network
    QString order;
    for (auto &e : m_orders) {
        if (!e.order.compare("hd")) {
            order.append("c");
        } else if (!e.order.compare("cdrom")) {
            order.append("d");
        } else if (!e.order.compare("network")) {
            order.append("n");
        }
    }
    if (order.length() > 0) {
        qemuCommand << "-boot";
        qemuCommand << QString("order=%1").arg(order);
    }

    // qemuCommand << "-monitor"
    //             << QString("tcp:localhost:%1,server,nowait")
    //                .arg(m_monitor_listen);
    qemuCommand << "-monitor";
    qemuCommand << "stdio";

    qemuCommand << "-name";
    qemuCommand << QString("%1_%2").arg(m_name).arg(m_domain_id);

    qemuCommand << "-m";
    qemuCommand << QString("size=%1m").arg(m_current_memory);

    int cpu_number = m_cpu_sockets.toInt() * m_cpu_cores.toInt() *
                     m_cpu_threads.toInt();
    if (cpu_number > 0) {
        qemuCommand << "-smp";
        qemuCommand << QString("%1,sockets=%2,cores=%3,threads=%4")
                       .arg(cpu_number)
                       .arg(m_cpu_sockets)
                       .arg(m_cpu_cores)
                       .arg(m_cpu_threads);
    }

    qemuCommand << "-accel";
    qemuCommand << m_accelerator;

    for (auto &e : m_cdroms) {
        if (e.file_path.length() <= 0) {
            continue;
        }
        qemuCommand << "-drive";
        qemuCommand << QString("media=cdrom,file=%1")
                .arg(e.file_path);
    }

    for (auto &e : m_disks) {
        if (e.file_path.length() <= 0) {
            continue;
        }
        QString disk_format;
        if (e.driver_type.length() > 0) {
            disk_format = QString("format=%1,").arg(e.driver_type);
        }
        qemuCommand << "-drive";
        qemuCommand << QString("media=disk,aio=threads,if=%1,%2file=%3")
                .arg("virtio")
                .arg(disk_format)
                .arg(e.file_path);
    }

    qemuCommand << "-net";
    qemuCommand << "nic";

    qemuCommand << "-net";
    qemuCommand << QString("user,hostfwd=tcp::%1-:22")
                   .arg(m_ssh_listen);

    qemuCommand << "-display";
    qemuCommand << "gtk";
    // qemuCommand << "-vnc";

    QString program = QDir::toNativeSeparators(m_qemu_binary_path);

    qDebug() << "qemu: " << program;
    qDebug() << program << qemuCommand;

    delete m_process;
    m_process = new QProcess();   

    connect(m_process, &QProcess::stateChanged,
            this, &VirtualMachine::recv_process_changed);

    m_process->start(program, qemuCommand);

}

void VirtualMachine::recv_process_changed(QProcess::ProcessState state)
{
    qDebug() << "process state:" << state;
    switch (state) {
        case QProcess::Starting:
            m_state = VMState(VMState::StateStarting);
            break;
        case QProcess::Running:
            m_state = VMState(VMState::StateRunning);
            break;
        case QProcess::NotRunning:
            m_state = VMState(VMState::StateShutdown);
            m_ssh_listen = 0;
            break;
    }

    emit send_vm_state_changed(m_name);
}

void VirtualMachine::stop()
{
    if (m_process && m_process->state() != QProcess::NotRunning){
        m_process->terminate();
    }
    delete m_process;
}

QString VirtualMachine::to_xml_string()
{    
    VirtualMachineDom vmDom = VirtualMachineDom();

    vmDom.set_accelerator(m_accelerator);
    vmDom.set_emulator(m_qemu_binary_path);
    vmDom.set_domain_id(m_domain_id);
    vmDom.set_name(m_name);
    vmDom.set_current_memory(m_current_memory);
    vmDom.set_cpu_sockets(m_cpu_sockets);
    vmDom.set_cpu_cores(m_cpu_cores);
    vmDom.set_cpu_threads(m_cpu_threads);
    vmDom.set_os_type(m_os_type);

    for (auto &d : m_disks) {
        if (d.file_path.length() > 0) {
            vmDom.append_disk(d.file_path, d.driver_type);
        }
    }

    for (auto &d : m_cdroms) {
        if (d.file_path.length() > 0) {
            vmDom.append_cdrom(d.file_path);
        }
    }

    QStringList orders;
    for (auto &d : m_orders) {
        if (d.order.length() > 0) {
            orders.append(d.order);
        }
    }
    vmDom.set_boot_order(orders);

    return vmDom.to_xml_string();
}
