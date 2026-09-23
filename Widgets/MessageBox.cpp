#include "MessageBox.h"

#include <QTimer>

MessageBox::MessageBox(QString msg, StandardButtons buttons, QWidget *parent) : QMessageBox(parent)
{
    setText(msg);
    setStandardButtons(buttons);
    setAttribute(Qt::WA_DeleteOnClose);
}

void MessageBox::showDelayed() { QTimer::singleShot(0, this, &MessageBox::show); }