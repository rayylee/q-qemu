#ifndef VIRTUAL_MACHINE_DOM_H
#define VIRTUAL_MACHINE_DOM_H

#include <QString>
#include <QDebug>
#include <QFile>
#include <QDomDocument>

class VirtualMachineDom
{
public:
    VirtualMachineDom();
    explicit VirtualMachineDom(const QString& xml_string);
    explicit VirtualMachineDom(QFile& f);
    ~VirtualMachineDom();

private:
    QDomElement _add_devices_element();
    QDomElement _add_cpu_element();

public:
    void set_accelerator(const QString&);
    void set_domain_id(const QString&);
    void set_emulator(const QString&);
    void set_name(const QString&);
    void set_current_memory(const QString&);
    void set_cpu_sockets(const QString&);
    void set_cpu_cores(const QString&);
    void set_cpu_threads(const QString&);
    void set_os_type(const QString&);
    void append_disk(const QString&, const QString&);
    void append_cdrom(const QString&);
    void set_boot_order(QStringList);

    QString get_accelerator();
    QString get_domain_id();
    QString get_emulator();
    QString get_name();
    QString get_current_memory();
    QString get_cpu_sockets();
    QString get_cpu_cores();
    QString get_cpu_threads();
    QString get_os_type();
    int get_disk_count();
    int get_cdrom_count();
    QString get_disk_type(int index);
    QString get_disk_file(int index);
    QString get_cdrom_file(int index);
    QStringList get_boot_order();

    QString to_xml_string();

private:
    QDomDocument m_doc;
    QDomElement m_root;
};

#endif // VIRTUAL_MACHINE_DOM_H
