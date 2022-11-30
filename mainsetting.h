#ifndef MAIN_SETTING_H
#define MAIN_SETTING_H

#include <QDialog>

namespace Ui {
class MainSetting;
}

class MainSetting : public QDialog
{
    Q_OBJECT

public:
    explicit MainSetting(QWidget *parent = nullptr);
    ~MainSetting() override;

    void set_app_dir(QString);

signals:
    void send_qemu_dir(QString, QString);

private slots:
    void on_confirmButtonBox_accepted();

    void on_choseQemuDirButton_clicked();

private:
    Ui::MainSetting *ui;

    QString m_app_dir;
    QString m_qemu_dir;
    QString m_qemu_binary_path;
};

#endif // MAIN_SETTING_H
