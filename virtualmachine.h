#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <QString>
#include <QProcess>
#include <QTcpSocket>
#include <utility>

#define MAX_DISK_COUNT  10
#define MAX_CDROM_COUNT 10

struct VMDisk {
    QString driver_type;
    QString file_path;
};

struct VMCdrom {
    QString file_path;
};

class VirtualMachine
{

public:
    VirtualMachine();
    explicit VirtualMachine(const QString&);
    ~VirtualMachine();

    void start(QString& ssh_port, QString& monitor_port);

    QString to_xml_string();

    void set_domain_id(QString in) { m_domain_id = std::move(in); }
    void set_qemu_bin_path(QString in) { m_qemu_binary_path = std::move(in); }
    void set_name(QString in) { m_name = std::move(in); }
    void set_os_type(QString in) { m_os_type = std::move(in); }
    void set_disk(int index, QString file_path, QString driver_type) {
        m_disks[index].file_path = std::move(file_path);
        m_disks[index].driver_type = std::move(driver_type);
    }
    void set_cdrom(int index, QString file_path) {
        m_cdroms[index].file_path = std::move(file_path);
    }

    QString name() { return m_name; }
    QString domain_id() { return m_domain_id; }

private:
    QProcess *m_process{};
    uint m_ssh_listen{};
    uint m_monitor_listen{};

    QString m_domain_id;
    QString m_accelerator;
    QString m_qemu_binary_path;
    QString m_name;
    QString m_os_type;
    VMDisk m_disks[MAX_DISK_COUNT];
    VMCdrom m_cdroms[MAX_CDROM_COUNT];
};


#endif // VIRTUAL_MACHINE_H

