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
    VirtualMachineDom(QString xml_string);
    VirtualMachineDom(QFile& f);
    ~VirtualMachineDom();

private:
    QDomElement _add_devices_element();

public:
    void set_accelerator(QString);
    void set_domain_id(QString);
    void set_domainid(QString);
    void set_emulator(QString);
    void set_name(QString);
    void set_os_type(QString);
    void append_disk(QString, QString);
    void append_cdrom(QString);

    QString get_accelerator();
    QString get_domain_id();
    QString get_emulator();
    QString get_name();
    QString get_os_type();
    int get_disk_count();
    int get_cdrom_count();
    QString get_disk_type(int index);
    QString get_disk_file(int index);
    QString get_cdrom_file(int index);

    QString to_xml_string();

private:
    QDomDocument m_doc;
    QDomElement m_root;
};

#endif // VIRTUALMACHINEDOM_H

QString get_disk_type();
