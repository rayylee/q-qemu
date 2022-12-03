#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <QString>
#include <QProcess>
#include <QTcpSocket>
#include <utility>

#define MAX_DISK_COUNT  10
#define MAX_CDROM_COUNT 10
#define MAX_ORDER_COUNT 5

class VMState {

public:
    enum PatternType {
        StateStarting,
        StateRunning,
        StateShutdown,
    };

public:
    VMState(){
        m_value = StateShutdown;
    }
    explicit VMState(PatternType t){
        m_value = t;
    }   

    PatternType value() {
        return m_value;
    }

    QString to_string() {
        switch (m_value) {
        case StateStarting:
            return QString("Starting");
        case StateRunning:
            return QString("Running");
        case StateShutdown:
            return QString("Shutdown");
        }
        return QString("Shutdown");
    }

private:
    PatternType m_value;
};

struct VMDisk {
    QString driver_type;
    QString file_path;
};

struct VMCdrom {
    QString file_path;
};

struct BootOrder {
    QString order;
};

class VirtualMachine : public QObject
{
    Q_OBJECT

public:
    VirtualMachine();
    explicit VirtualMachine(const QString&);
    ~VirtualMachine() override;

    void start(const QString& ssh_port, const QString& vnc_port);
    void stop();

    QString to_xml_string();

    void set_domain_id(QString in) { m_domain_id = std::move(in); }
    void set_accelerator(QString in) { m_accelerator = std::move(in); }
    void set_qemu_bin_path(QString in) { m_qemu_binary_path = std::move(in); }
    void set_name(QString in) { m_name = std::move(in); }
    void set_current_memory(QString in) { m_current_memory = std::move(in); }
    void set_cpu_sockets(QString in) { m_cpu_sockets = std::move(in); }
    void set_cpu_cores(QString in) { m_cpu_cores = std::move(in); }
    void set_cpu_threads(QString in) { m_cpu_threads = std::move(in); }
    void set_os_type(QString in) { m_os_type = std::move(in); }
    void set_disk(int index, QString file_path, QString driver_type) {
        m_disks[index].file_path = std::move(file_path);
        m_disks[index].driver_type = std::move(driver_type);
    }
    void set_cdrom(int index, QString file_path) {
        m_cdroms[index].file_path = std::move(file_path);
    }
    void set_boot_order(const QStringList& orders) {
        for (auto & e : m_orders) {
            e.order.clear();
        }
        for (int i = 0; i < orders.length(); i++) {
            m_orders[i].order = orders.at(i);
        }
    }

    QString name() { return m_name; }
    uint ssh_listen_port() { return m_ssh_listen; }
    uint vnc_listen_port() { return m_vnc_listen; }
    QString cpu_sockets() { return m_cpu_sockets; }
    QString cpu_cores() { return m_cpu_cores; }
    QString cpu_threads() { return m_cpu_threads; }
    QString current_memory() { return m_current_memory; }
    QString accelerator() { return m_accelerator; }
    VMState state() { return m_state; }
    QString domain_id() { return m_domain_id; }
    QString qemu_binary_path() { return m_qemu_binary_path; }
    QString disk(int index) { return m_disks[index].file_path; }
    QString cdrom(int index) { return m_cdroms[index].file_path; }

    qint64 process_write(const char *data, qint64 len) {
        return m_process->write(data, len);
    }
    qint64 process_id() {
        if (m_process && m_process->state() == QProcess::Running) {
            return m_process->processId();
        }
        return 0;
    }
signals:
    void send_vm_state_changed(QString);

private slots:
    void recv_process_changed(QProcess::ProcessState state);

private:
    QProcess *m_process = nullptr;
    VMState m_state;
    uint m_ssh_listen;
    uint m_vnc_listen;

    QString m_domain_id;
    QString m_accelerator;
    QString m_qemu_binary_path;
    QString m_name;
    QString m_current_memory;
    QString m_cpu_sockets;
    QString m_cpu_cores;
    QString m_cpu_threads;
    QString m_os_type;
    BootOrder m_orders[MAX_ORDER_COUNT];
    VMDisk m_disks[MAX_DISK_COUNT];
    VMCdrom m_cdroms[MAX_CDROM_COUNT];
};


#endif // VIRTUAL_MACHINE_H

