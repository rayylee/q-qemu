#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDomDocument>
#include <QProcess>
#include <QTcpSocket>

#include "virtualmachine.h"
#include "virtualmachinedom.h"

VirtualMachine::VirtualMachine()
{

}

VirtualMachine::VirtualMachine(const QString& xml_path)
{   
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
    m_os_type = dom.get_os_type();
    int disk_count = dom.get_disk_count();
    for (int i = 0; i < disk_count && i < MAX_DISK_COUNT ; i++) {
        m_disks[i].driver_type = dom.get_disk_type(i);
        m_disks[i].file_path = dom.get_disk_file(i);
    }
    int cdrom_count = dom.get_cdrom_count();
    for (int i = 0; i < cdrom_count && i < MAX_CDROM_COUNT ; i++) {
        m_cdroms[i].file_path = dom.get_disk_file(i);
    }
}

VirtualMachine::~VirtualMachine()
{

}

void VirtualMachine::start(QString& ssh_port, QString& monitor_port)
{
    QStringList qemuCommand;

    m_ssh_listen = ssh_port.toUInt() + m_domain_id.toUInt() - 1;
    m_monitor_listen = monitor_port.toUInt() + m_domain_id.toUInt() - 1;

    qemuCommand << "-monitor"
                << QString("tcp:localhost:%1,server,nowait")
                   .arg(m_monitor_listen);

    qemuCommand << "-name";
    qemuCommand << QString("%1_%2").arg(m_name).arg(m_domain_id);

    qemuCommand << "-accel";
    qemuCommand << m_accelerator;

    qemuCommand << "-hda";
    qemuCommand << m_disks[0].file_path;

    QString program = QDir::toNativeSeparators(m_qemu_binary_path);

    qDebug() << "qemu: " << m_qemu_binary_path;
    qDebug() << program << qemuCommand;

    m_process = new QProcess();
    m_process->start(program, qemuCommand);
}

QString VirtualMachine::to_xml_string()
{    
    VirtualMachineDom vmDom = VirtualMachineDom();

    vmDom.set_emulator(m_qemu_binary_path);
    vmDom.set_domain_id(m_domain_id);
    vmDom.set_name(m_name);
    vmDom.set_os_type(m_os_type);

    if (m_disks[0].file_path.length() > 0) {
        vmDom.append_disk(m_disks[0].file_path, "qcow2");
    }

    if (m_disks[1].file_path.length() > 0) {
        vmDom.append_disk(m_disks[1].file_path, "qcow2");
    }

    if (m_cdroms[0].file_path.length() > 0) {
        vmDom.append_cdrom(m_cdroms[0].file_path);
    }

    return vmDom.to_xml_string();
}
