#ifndef GLOBALSETTING_H
#define GLOBALSETTING_H

#include <QString>

class GlobalSetting
{
public:
    GlobalSetting(QString app_dir);
    ~GlobalSetting();

public:
    void save_config();

    void set_qemu_dir(QString in) { m_qemu_dir = in; }
    void search_qemu_binary();

    QString qemu_dir() { return m_qemu_dir; }
    QString qemu_binary_path() { return m_qemu_binary_path; }

private:
    QString m_config_path;

    QString m_qemu_dir;
    QString m_qemu_binary_path;
};

#endif
