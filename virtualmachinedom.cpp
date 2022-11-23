#include "virtualmachinedom.h"

VirtualMachineDom::~VirtualMachineDom()
{

}

VirtualMachineDom::VirtualMachineDom()
{
    m_root = m_doc.createElement("domain");
    m_root.setAttribute("type", "qemu");
    m_doc.appendChild(m_root);
}

VirtualMachineDom::VirtualMachineDom(QString xml_string)
{
    if (!m_doc.setContent(xml_string)) {
        return;
    }
    m_root = m_doc.documentElement();
}

QString VirtualMachineDom::get_name()
{
    QDomElement name_element;
    QDomNodeList os_nodes = m_root.childNodes();
    for(int i = 0; i < os_nodes.count(); i++) {
        QDomNode node = os_nodes.at(i);
        if (!node.nodeName().compare("name")) {
            name_element = os_nodes.at(i).toElement();
            return name_element.firstChild().nodeValue();
        }
    }

    return QString("");
}

QString VirtualMachineDom::get_os_type()
{
    QDomElement os_element;
    QDomNodeList os_nodes = m_root.childNodes();
    for(int i = 0; i < os_nodes.count(); i++) {
        QDomNode node = os_nodes.at(i);
        if (!node.nodeName().compare("os")) {
            os_element = os_nodes.at(i).toElement();
        }
    }

    if (!os_element.isNull()) {
        QDomElement system_element;
        QDomNodeList system_nodes = os_element.childNodes();
        for(int i = 0; i < system_nodes.count(); i++) {
            QDomNode node = system_nodes.at(i);
            if (node.nodeName() == "system") {
                system_element = system_nodes.at(i).toElement();
                break;
            }
        }

        if (!system_element.isNull()) {
            return system_element.firstChild().nodeValue();
        }

    }

    return QString("");
}

int VirtualMachineDom::get_disk_count()
{
    int count = 0;

    QDomElement devices_element;
    QDomNodeList nodes = m_root.childNodes();
    for(int i = 0; i < nodes.count(); i++) {
        QDomNode node = nodes.at(i);
        if (!node.nodeName().compare("devices")) {
            devices_element = nodes.at(i).toElement();
        }
    }

    if (devices_element.isNull()) {
        return count;
    }


    QDomNodeList devices_nodes = devices_element.childNodes();
    for(int i = 0; i < devices_nodes.count(); i++) {
        QDomElement ele = devices_nodes.at(i).toElement();
        if (ele.hasAttribute("device") &&
                !ele.attribute("device").compare("disk")) {
            count++;
        }
    }

    return count;
}

void VirtualMachineDom::set_accelerator(QString accel)
{
    m_root.setAttribute("type", accel);
}

void VirtualMachineDom::set_name(QString name)
{
    QDomElement name_element;
    QDomNodeList nodes = m_root.childNodes();
    for(int i = 0; i < nodes.count(); i++) {
        QDomNode node = nodes.at(i);
        if (!node.nodeName().compare("name")) {
            name_element = nodes.at(i).toElement();
            break;
        }
    }
    if (name_element.isNull()) {
        name_element = m_doc.createElement("name");
        name_element.appendChild(m_doc.createTextNode(name));
        m_root.appendChild(name_element);
    } else {
        name_element.firstChild().setNodeValue(name);
    }
}

void VirtualMachineDom::set_os_type(QString os_type)
{
    QDomElement os_element;
    QDomNodeList os_nodes = m_root.childNodes();
    for(int i = 0; i < os_nodes.count(); i++) {
        QDomNode node = os_nodes.at(i);
        if (node.nodeName() == "os") {
            os_element = os_nodes.at(i).toElement();
            break;
        }
    }
    if (os_element.isNull()) {
        os_element = m_doc.createElement("os");
        m_root.appendChild(os_element);
    }

    QDomElement system_element;
    QDomNodeList system_nodes = os_element.childNodes();
    for(int i = 0; i < system_nodes.count(); i++) {
        QDomNode node = system_nodes.at(i);
        if (node.nodeName() == "system") {
            system_element = system_nodes.at(i).toElement();
            break;
        }
    }
    if (system_element.isNull()) {
        system_element = m_doc.createElement("system");
        system_element.appendChild(m_doc.createTextNode(os_type));
        os_element.appendChild(system_element);
    } else {
        system_element.firstChild().setNodeValue(os_type);
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

    m_root.appendChild(devices_element);

    QDomElement devices_disk = m_doc.createElement("disk");
    devices_disk.setAttribute("type", "file");
    devices_disk.setAttribute("device", "disk");
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

    m_root.appendChild(devices_element);

    QDomElement devices_cdrom = m_doc.createElement("disk");
    devices_cdrom.setAttribute("type", "file");
    devices_cdrom.setAttribute("device", "cdrom");
    devices_element.appendChild(devices_cdrom);

    QDomElement disk_source = m_doc.createElement("source");
    disk_source.setAttribute("file", cdrom_path);
    devices_cdrom.appendChild(disk_source);
}

QString VirtualMachineDom::to_xml_string()
{
    QString xml_string;

    QTextStream out_stream(&xml_string);
    m_doc.save(out_stream, 4);

    return xml_string;
}
