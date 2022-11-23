#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QDirIterator>

#include "global_setting.h"

GlobalSetting::GlobalSetting(QString app_dir)
{
    QDir dir_app(app_dir);

    if(!dir_app.exists()) {
        dir_app.mkpath(app_dir);
    }

    m_config_path = QDir::toNativeSeparators(
                dir_app.absoluteFilePath("config.ini"));

    QSettings setting(m_config_path, QSettings::IniFormat);

    setting.beginGroup("global");

    foreach(QString key, setting.allKeys()) {
        if (!key.compare("qemu-dir")) {
            m_qemu_dir = setting.value(key).toString();
        } else if (!key.compare("qemu-bin")) {
            m_qemu_binary_path = setting.value(key).toString();
        }
    }

    setting.endGroup();
}

GlobalSetting::~GlobalSetting()
{

}

void GlobalSetting::search_qemu_binary()
{
    m_qemu_binary_path = "-";

    if (m_qemu_dir.isEmpty()) {
        return;
    }

    QDirIterator it(m_qemu_dir, QStringList() << "qemu-system-*",
                    QDir::NoFilter, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        it.next();
        if (it.fileName().contains("x86_64w")) {
            QDir dir_path(m_qemu_dir);
            m_qemu_binary_path = QDir::toNativeSeparators(
                        dir_path.absoluteFilePath(it.fileName()));
        }
    }
}

void GlobalSetting::save_config()
{
    QSettings setting(m_config_path, QSettings::IniFormat);

    setting.beginGroup("global");

    setting.setValue("qemu-dir", m_qemu_dir);
    setting.setValue("qemu-bin", m_qemu_binary_path);

    setting.endGroup();
}
