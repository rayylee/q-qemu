#ifndef VIRTUALMACHINEDOM_H
#define VIRTUALMACHINEDOM_H

#include <QString>
#include <QDebug>
#include <QFile>
#include <QDomDocument>

class VirtualMachineDom
{
public:
    VirtualMachineDom();
    ~VirtualMachineDom();

private:
    QDomElement _add_devices_element();

public:
    void set_accelerator(QString);
    void set_name(QString);
    void set_os_type(QString);
    void append_disk(QString, QString);
    void append_cdrom(QString);

    QString to_xml_string();

private:
    QDomDocument m_doc;
    QDomElement m_root;
};

#endif // VIRTUALMACHINEDOM_H
