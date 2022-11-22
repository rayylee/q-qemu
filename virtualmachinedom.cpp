#include "virtualmachinedom.h"

VirtualMachineDom::VirtualMachineDom()
{
    m_root = m_doc.createElement("domain");
    m_root.setAttribute("type", "qemu");
    m_doc.appendChild(m_root);
}

VirtualMachineDom::~VirtualMachineDom()
{

}

void VirtualMachineDom::set_accelerator(QString accel)
{
    m_root.setAttribute("type", accel);
}

void VirtualMachineDom::set_name(QString name)
{
    QDomNodeList nodes = m_root.elementsByTagName("name");
    if (nodes.length() <= 0) {
        QDomElement name_element = m_doc.createElement("name");
        name_element.appendChild(m_doc.createTextNode(name));
        m_root.appendChild(name_element);
    } else {
        nodes.at(0).firstChild().setNodeValue(name);
    }
}

void VirtualMachineDom::set_os_type(QString os_type)
{
    QDomElement os_element;

    QDomNodeList os_nodes = m_root.elementsByTagName("os");
    if (os_nodes.length() <= 0) {
        os_element = m_doc.createElement("os");
        m_root.appendChild(os_element);
    } else {
        os_element = os_nodes.at(0).toElement();
    }

    QDomNodeList system_nodes = os_element.elementsByTagName("system");
    if (system_nodes.length() <= 0) {
        QDomElement system_element = m_doc.createElement("system");
        system_element.appendChild(m_doc.createTextNode(os_type));
        os_element.appendChild(system_element);
    } else {
        system_nodes.at(0).firstChild().setNodeValue(os_type);
    }
}

QDomElement VirtualMachineDom::_add_devices_element()
{
    QDomNodeList devices_nodes = m_root.elementsByTagName("devices");
    if (devices_nodes.length() <= 0) {
        QDomElement devices_element = m_doc.createElement("devices");
        m_root.appendChild(devices_element);
        return devices_element;
    }

    return devices_nodes.at(0).toElement();
}

void VirtualMachineDom::append_disk(QString disk_path, QString driver_type)
{
    QDomElement devices_element = _add_devices_element();

    devices_element.setAttribute("type", "file");
    devices_element.setAttribute("device", "disk");

    m_root.appendChild(devices_element);

    QDomElement devices_disk = m_doc.createElement("disk");
    devices_element.appendChild(devices_disk);

    QDomElement disk_driver = m_doc.createElement("driver");
    disk_driver.setAttribute("name", "qemu");
    disk_driver.setAttribute("type", driver_type);
    devices_disk.appendChild(disk_driver);

    QDomElement disk_source = m_doc.createElement("source");
    disk_source.setAttribute("file", disk_path);
    devices_disk.appendChild(disk_source);
}

void VirtualMachineDom::append_cdrom(QString cdrom_path)
{
    QDomElement devices_element = _add_devices_element();

    devices_element.setAttribute("type", "file");
    devices_element.setAttribute("device", "cdrom");

    m_root.appendChild(devices_element);

    QDomElement devices_disk = m_doc.createElement("disk");
    devices_element.appendChild(devices_disk);

    QDomElement disk_source = m_doc.createElement("source");
    disk_source.setAttribute("file", cdrom_path);
    devices_disk.appendChild(disk_source);
}

QString VirtualMachineDom::to_xml_string()
{
    QString xml_string;

    QTextStream out_stream(&xml_string);
    m_doc.save(out_stream, 4);

    return xml_string;
}
