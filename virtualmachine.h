#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include <QString>

struct VMDisk {
    QString driver_type;
    QString file_path;
};

class VirtualMachine
{

public:
    VirtualMachine();
    VirtualMachine(QString);
    ~VirtualMachine();

    QString to_xml_string();

    void set_qemu_path(QString in) { m_qemu_path = in; }
    void set_name(QString in) { m_name = in; }
    void set_os_type(QString in) { m_os_type = in; }
    void set_disk(int index, QString file_path, QString driver_type) {
        m_disks[index].file_path = file_path;
        m_disks[index].driver_type = driver_type;
    }
    void set_cdrom(QString in) { m_cdrom = in; }

    QString qemu_path() { return m_qemu_path; }
    QString name() { return m_name; }
    QString os_type() { return m_os_type; }
    QString disk_path(int index) { return m_disks[index].file_path; }
    QString disk_type(int index) { return m_disks[index].driver_type; }
    QString cdrom() { return m_cdrom; }

private:
    QString m_qemu_path;
    QString m_name;
    QString m_os_type;
    VMDisk m_disks[2];
    QString m_cdrom;
};


#endif // VIRTUALMACHINE_H
