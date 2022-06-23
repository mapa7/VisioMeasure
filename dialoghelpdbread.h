#ifndef DIALOGHELPDBREAD_H
#define DIALOGHELPDBREAD_H

#include <QDialog>
#include <QLabel>

namespace Ui {
class DialogHelpDBRead;
}

class DialogHelpDBRead : public QDialog
{
    Q_OBJECT

public:
    explicit DialogHelpDBRead(QWidget *parent = nullptr);
    ~DialogHelpDBRead();
public slots:
    void UpdateDispValues(bool boolFromPlc[],float floatFromPlc[]);

private slots:
    void on_pushButtonCloseDialog_clicked();

private:
    bool boolFromPlc[8]={false};
    float floatFromPlc[10]={0.0};

    Ui::DialogHelpDBRead *ui;
    QList<QLabel *> labelsBit;
    QList<QLabel *> labelsFloat;

    void CreateQListFromUILabels();
    void UpdateWindow();
};

#endif // DIALOGHELPDBREAD_H
