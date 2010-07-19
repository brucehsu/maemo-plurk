#include "clicklabel.h"

const QString border = "border: 1px solid white";

clickLabel::clickLabel(QWidget *parent) :
    QLabel(parent)
{
    this->setStyleSheet(border);
}

clickLabel::clickLabel(QString text) {

    this->setText(text);
    this->setStyleSheet(border);
}

void clickLabel::mousePressEvent(QMouseEvent *ev) {
    this->setStyleSheet(border + "; background-color: blue");
}

void clickLabel::mouseReleaseEvent( QMouseEvent * ev ) {
    this->setStyleSheet(border);
    emit clicked();
}
