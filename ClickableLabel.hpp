#ifndef CLICKABLELABEL_HPP
#define CLICKABLELABEL_HPP

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget *parent = nullptr) : QLabel(parent) {}

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override {
        QLabel::mousePressEvent(event);
        emit clicked();
    }
};

#endif // CLICKABLELABEL_HPP
