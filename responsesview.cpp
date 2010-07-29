#include "responsesview.h"
#include "ui_responsesview.h"

ResponsesView::ResponsesView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResponsesView)
{
    ui->setupUi(this);
}

ResponsesView::~ResponsesView()
{
    delete ui;
}
