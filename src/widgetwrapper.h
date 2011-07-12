#ifndef WIDGETWRAPPER_H
#define WIDGETWRAPPER_H

#include <QWidget>
#include <QTimer>

class WidgetWrapper : public QWidget
{
    Q_OBJECT
public:
    WidgetWrapper(QWidget *prey);
    ~WidgetWrapper();

protected:
    void resizeEvent(QResizeEvent *e);

signals:
    void shrink();
    void grow();
    void toggleFullScreen();

public slots:
    void handleResize();
    void resetUI();

private:
    QTimer resizeSettleTimer;
    QWidget *m_prey;
    QWidget *viewport;
};

#endif // WIDGETWRAPPER_H
