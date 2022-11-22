#include <QDebug>
#include <QFile>
#include <QDomDocument>

#include "virtualmachine.h"
#include "virtualmachinedom.h"

VirtualMachine::VirtualMachine()
{

}

VirtualMachine::VirtualMachine(QString xml_path)
{
    QFile file(xml_path);
    file.open(QIODevice::ReadOnly);
    QByteArray all_array=file.readAll();
    QString all_string = QString(all_array);
    file.close();

    qDebug().noquote() << "Read: \n" << all_string;
}

VirtualMachine::~VirtualMachine()
{

}

QString VirtualMachine::to_xml_string()
{    
    VirtualMachineDom vmDom = VirtualMachineDom();

    vmDom.set_name(m_name);
    vmDom.set_os_type(m_os_type);

    if (m_disks[0].file_path.length() > 0) {
        vmDom.append_disk(m_disks[0].file_path, "qcow2");
    }

    if (m_disks[1].file_path.length() > 0) {
        vmDom.append_disk(m_disks[1].file_path, "qcow2");
    }

    if (m_cdrom.length() > 0) {
        vmDom.append_cdrom(m_cdrom);
    }

//    QString xml_string;
//    QDomDocument doc;

//    QDomElement root_element = doc.createElement("domain");
//    root_element.setAttribute("type", "kvm");
//    doc.appendChild(root_element);

//    QDomElement name_element = doc.createElement("name");
//    name_element.appendChild(doc.createTextNode(m_name));
//    root_element.appendChild(name_element);

//    QDomElement os_element = doc.createElement("os");
//    QDomElement os_system_element = doc.createElement("system");
//    os_system_element.appendChild(doc.createTextNode(m_os_type));
//    os_element.appendChild(os_system_element);
//    root_element.appendChild(os_element);

//    QDomElement devices_element = doc.createElement("devices");
//    devices_element.setAttribute("type", "file");
//    devices_element.setAttribute("device", "disk");
//    root_element.appendChild(devices_element);

//    QDomElement devices_disk = doc.createElement("disk");
//    devices_element.appendChild(devices_disk);

//    QDomElement disk_source = doc.createElement("source");
//    disk_source.setAttribute("file", m_disks[0]);
//    devices_disk.appendChild(disk_source);

//    QTextStream out_stream(&xml_string);
//    doc.save(out_stream, 4);

    return vmDom.to_xml_string();
}
