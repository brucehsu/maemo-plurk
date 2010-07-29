#ifndef RESPONSESVIEW_H
#define RESPONSESVIEW_H

#include <QWidget>

namespace Ui {
    class ResponsesView;
}

class ResponsesView : public QWidget
{
    Q_OBJECT

public:
    explicit ResponsesView(QWidget *parent = 0);
    ~ResponsesView();

private:
    Ui::ResponsesView *ui;
};

#endif // RESPONSESVIEW_H
