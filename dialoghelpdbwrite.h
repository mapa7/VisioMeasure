#ifndef DIALOGHELPDBWRITE_H
#define DIALOGHELPDBWRITE_H

#include <QDialog>
#include <QLabel>

namespace Ui {
class DialogHelpDBWrite;
}

class DialogHelpDBWrite : public QDialog
{
    Q_OBJECT

public:
    explicit DialogHelpDBWrite(QWidget *parent = nullptr);
    ~DialogHelpDBWrite();
public slots:
    void UpdateDispValues(bool boolToPlc[],float floatToPlc[]);

private slots:
    void on_pushButtonCloseDialog_clicked();

private:
    bool boolToPlc[8]={false};
    float floatToPlc[10]={0.0};

    Ui::DialogHelpDBWrite *ui;
    QList<QLabel *> labelsBit;
    QList<QLabel *> labelsFloat;

    void CreateQListFromUILabels();
    void UpdateWindow();
};

#endif // DIALOGHELPDBWRITE_H
