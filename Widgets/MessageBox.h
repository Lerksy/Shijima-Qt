#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QMessageBox>

class MessageBox : public QMessageBox
{
    Q_OBJECT
public:
    MessageBox(QString msg, StandardButtons buttons, QWidget *parent = nullptr);

    void showDelayed();
};

#endif //MESSAGEBOX_H
