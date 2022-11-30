#ifndef GLOBAL_SETTING_H
#define GLOBAL_SETTING_H

#include <QString>
#include <QDebug>
#include <QDir>
#include <utility>

class GlobalSetting
{
public:
    explicit GlobalSetting(const QString& app_dir);
    ~GlobalSetting();

public:
    void save_config();

    void set_qemu_dir(QString in) { m_qemu_dir = std::move(in); }
    void set_bitmap_string(QString in) { m_bitmap_string = std::move(in); }
    void set_monitor_port(QString in) { m_monitor_port = std::move(in); }
    void set_ssh_port(QString in) { m_ssh_port = std::move(in); }
    void search_qemu_binary();    

    QString qemu_dir() {
        if (m_qemu_dir.length() <= 0) {
            _default_qemu_dir();
        }
        return m_qemu_dir;
    }

    void _default_qemu_dir()
    {
        m_qemu_dir = QDir::toNativeSeparators("D:/Program Files/qemu");
    }

    QString qemu_binary_path() {
        if (m_qemu_dir.length() <= 0) {
            _default_qemu_dir();
        }
        if (m_qemu_binary_path.length() <= 0){
            search_qemu_binary();
        }
        return m_qemu_binary_path;
    }

    QString monitor_port() {
        if (m_monitor_port.length() <= 0){
            m_monitor_port = QString("6000");
        }
        return m_monitor_port;
    }

    QString ssh_port() {
        if (m_ssh_port.length() <= 0){
            m_ssh_port = QString("5555");
        }
        return m_ssh_port;
    }

    QString bitmap_string() { return m_bitmap_string; }

private:
    QString m_config_path;

    QString m_qemu_dir;
    QString m_qemu_binary_path;
    QString m_bitmap_string;
    QString m_monitor_port;
    QString m_ssh_port;
};

class BitMap
{
public:
    explicit BitMap(const uint range = 0) {
        delete[] m_bits;

        m_count = range;
        if (m_count > 0) {
            m_count -= 1;
        }
        m_size = m_count / 32 + 1;
        m_bits = new uint[m_size];

        clear_all();
    }

    explicit BitMap(const QString& s) {
        delete[] m_bits;

        m_count = static_cast<int>(s.length());
        if (m_count > 0) {
            m_count -= 1;
        }
        m_size = m_count / 32 + 1;
        m_bits = new uint[m_size];

        clear_all();

        for (int i = 0; i < m_size; i++) {
            for (int j = 0; j < 32; j++) {
                int pos = i*32 + j;

                if (pos >= s.length()) {
                    break;
                }

                if (s.at(s.length() - pos - 1) == '1') {
                    set(pos);
                } else {
                    clear(pos);
                }
            }
        }

    }

    ~BitMap() {
        delete[] m_bits;
    }

    QString to_string() const {
        QString s;
        for (int i = 0; i < m_size; i++) {
            for (int j = 0; j < 32; j++) {
                if (m_bits[i] & (0x1 << j)) {
                    s = QString("1").append(s);
                } else {
                    s = QString("0").append(s);
                }
            }
        }
        return s;
    }

    void clear_all() const {
        for (int i = 0; i < static_cast<int>(m_size); i++) {
            m_bits[i] = 0;
        }
    }

    void set(const int& num) const {
        int index = num / 32;
        uint bit_index = num % 32;
        m_bits[index] |= 1 << bit_index;
    }

    void clear(const int& num) const{
        int index = num / 32;
        uint bit_index = num % 32;
        m_bits[index] &= ~(1 << bit_index);
    }

    bool get(const int& num) const {
        int index = num / 32;
        uint bit_index = num % 32;
        return (m_bits[index] >> bit_index) & 1;
    }

    int get_first_zero() const
    {
        int i = 0;
        int j = 0;
        for (; i < m_size; i++) {
            for (j = 0; j < 32; j++) {
                if (!(m_bits[i] & (0x1 << j))) {
                    return i*32 + j;
                }
            }
        }
        return -1;
    }

public:
    uint *m_bits = nullptr;
    int m_size;
    int m_count;
};

#endif // GLOBAL_SETTING_H
