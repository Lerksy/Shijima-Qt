#include "SandboxWidget.h"

SandboxWidget::SandboxWidget(QWidget *parent) : QWidget(parent, Qt::Window)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setAttribute(Qt::WA_DeleteOnClose);
    resize(640, 480);
    setObjectName("sandboxWindow");
    show();
}