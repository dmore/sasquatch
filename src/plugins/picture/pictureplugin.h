#ifndef PICTUREPLUGIN_H
#define PICTUREPLUGIN_H

#include <QObject>

#include "qmhplugin.h"
#include "picturemodel.h"

class PicturePlugin : public QMHPlugin
{
    Q_OBJECT
    Q_INTERFACES(QMHPlugin)
    Q_PROPERTY(QObject *model READ model NOTIFY modelChanged)

public:
    PicturePlugin();
    ~PicturePlugin() {}

    // accessed from QML
    QObject *model() const { return m_model; }
    
    void registerPlugin(QDeclarativeContext *context);
signals:
    void modelChanged();
    
private:
    PictureModel *m_model;
};

#endif // PICTUREPLUGIN_H
