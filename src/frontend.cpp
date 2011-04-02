/****************************************************************************

This file is part of the QtMediaHub project on http://www.gitorious.org.

Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).*
All rights reserved.

Contact:  Nokia Corporation (qt-info@nokia.com)**

You may use this file under the terms of the BSD license as follows:

"Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of Nokia Corporation and its Subsidiary(-ies) nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."

****************************************************************************/

#include "frontend.h"
#include "backend.h"

#include <QDebug>
#include <QFileInfo>
#include <QList>
#include <QVariant>

#include <QDeclarativeView>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QDeclarativeComponent>
#include <QApplication>
#include <QTimer>
#include <QTranslator>

#ifdef GLVIEWPORT
#include <QGLWidget>
#endif

#include <QHostInfo>

#include "qml-extensions/actionmapper.h"
#include "qml-extensions/mediaplayerhelper.h"
#include "qml-extensions/trackpad.h"
#include "rpc/rpcconnection.h"
#include "qmh-config.h"
#include "skin.h"

#include "dataproviders/proxymodel.h"
#include "dataproviders/dirmodel.h"
#include "qml-extensions/qmlfilewrapper.h"
#include "dataproviders/playlist.h"
#include "plugins/qmhplugin/qmhplugin.h"

#ifdef QMH_AVAHI
#include "qavahiservicepublisher.h"
#endif

class FrontendPrivate : public QObject
{
    Q_OBJECT
public:
    FrontendPrivate(Frontend *p);
    ~FrontendPrivate();

public slots:
    void handleResize();
    void resetUI();

public:
    QGraphicsView *centralWidget;
    QTranslator *frontEndTranslator;
    Skin *skin;
    QTimer resizeSettleTimer;
    const QRect defaultGeometry;
    bool overscanWorkAround;
    bool attemptingFullScreen;
    ActionMapper *actionMap;
    MediaPlayerHelper *mediaPlayerHelper;
    Trackpad *trackpad;
    Frontend *pSelf;
};

FrontendPrivate::FrontendPrivate(Frontend *p)
    : QObject(p),
      centralWidget(0),
      frontEndTranslator(0),
      defaultGeometry(0, 0, 1080, 720),
      attemptingFullScreen(false),
      actionMap(new ActionMapper(p)),
      mediaPlayerHelper(new MediaPlayerHelper(p)),
      trackpad(new Trackpad(p)),
      pSelf(p)
{
    resizeSettleTimer.setSingleShot(true);
    connect(&resizeSettleTimer, SIGNAL(timeout()), this, SLOT(handleResize()));
    overscanWorkAround = Config::isEnabled("overscan", false);
}

FrontendPrivate::~FrontendPrivate()
{
    Config::setValue("fullscreen", attemptingFullScreen);
    Config::setValue("last-skin", skin);

    delete centralWidget;
    centralWidget = 0;
}

void FrontendPrivate::handleResize()
{
    if (centralWidget)
        centralWidget->setFixedSize(pSelf->size());

    QGraphicsView *gv = qobject_cast<QGraphicsView*>(centralWidget);
    if (gv && Config::isEnabled("scale-ui", false)) {
        gv->resetMatrix();
        //Needs to be scaled by res of top level qml file
        gv->scale(qreal(pSelf->width())/1280, qreal(pSelf->height())/720);
    }
}

void FrontendPrivate::resetUI()
{
    QDeclarativeView *declarativeWidget = qobject_cast<QDeclarativeView*>(centralWidget);
    if (declarativeWidget) {
        QObject* coreObject = declarativeWidget->rootObject();
        coreObject->setProperty("state", "");
        //coreObject->setProperty("focus", true);
        QMetaObject::invokeMethod(coreObject, "resetFocus");
    }
}

Frontend::Frontend(QWidget *p)
    : QWidget(p),
      d(new FrontendPrivate(this))
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    setSkin(Config::value("last-skin", "").toString());
    connect(this, SIGNAL(resettingUI()), d, SLOT(resetUI()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::ALT + Qt::Key_Backspace), this, SIGNAL(resettingUI()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::ALT + Qt::Key_Down), this, SLOT(shrink()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::ALT + Qt::Key_Up), this, SLOT(grow()));
    new QShortcut(QKeySequence(Qt::ALT + Qt::Key_Return), this, SLOT(toggleFullScreen()));

#ifdef QMH_AVAHI
    QAvahiServicePublisher *publisher = new QAvahiServicePublisher(this);
    publisher->publish(QHostInfo::localHostName(), "_qmh._tcp", 1234, "Qt Media Hub JSON-RPCv2 interface");
#endif

    RpcConnection *connection = new RpcConnection(RpcConnection::Server, QHostAddress::Any, 1234, this);
    connection->registerObject(d->actionMap);
    connection->registerObject(d->mediaPlayerHelper);
    connection->registerObject(d->trackpad);

    installEventFilter(Backend::instance());
}

Frontend::~Frontend()
{
    Config::setEnabled("overscan", d->overscanWorkAround);

    if (d->overscanWorkAround)
        Config::setValue("overscan-geometry", geometry());
    else
        Config::setValue("window-geometry", geometry());

    delete d;
    d = 0;
    //Can't decide whether this is filthy or not
    Backend::destroy();
}

void Frontend::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    QPainter p(this);
    p.fillRect(rect(), Qt::black);
}

void Frontend::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e)
    d->resizeSettleTimer.start(300);
}

void Frontend::setSkin(const QString &name)
{
    static QSize nativeResolution = qApp->desktop()->screenGeometry(this).size();
    static QString nativeResolutionString = Config::value("native-res-override", QString("%1x%2").arg(nativeResolution.width()).arg(nativeResolution.height()));
    //http://en.wikipedia.org/wiki/720p
    //1440, 1080, 720, 576, 480, 360, 240
    static QHash<QString, QString> resolutionHash;
    resolutionHash["1440"] = "2560x1440";
    resolutionHash["1080"] = "1920x1080";
    resolutionHash["720"] = "1280x720";

    Backend *backend = Backend::instance();
    Skin *skin = 0;
    Skin *defaultSkin = 0;
    QString defaultSkinName = Config::value("default-skin", "confluence").toString();

    foreach (QObject *o, backend->skins()) {
        Skin *s = qobject_cast<Skin*>(o);
        if (s->name() == name)
            skin = s;
        if (s->name() == defaultSkinName)
            defaultSkin = s;
    }

    if (!skin)
        skin = defaultSkin;

    if (!skin)
        qFatal("Something has gone horribly awry, you want for skins");

    QFile skinConfig(skin->config());
    if (skinConfig.open(QIODevice::ReadOnly))
    {
        QHash<QString, QString> fileForResolution;
        QTextStream skinStream(&skinConfig);
        while(!skinStream.atEnd())
        {
            QStringList resolutionToFile = skinStream.readLine().split(":");
            if (resolutionToFile.count() == 2) {
                QString resolution =
                        resolutionHash.contains(resolutionToFile.at(0))
                        ? resolutionHash[resolutionToFile.at(0)]
                        : resolutionToFile.at(0);
                fileForResolution[resolution] = resolutionToFile.at(1);
            } else {
                qWarning() << "bad line in skin configuration";
            }
        }

        QString urlPath =
                fileForResolution.contains(nativeResolutionString)
                ? fileForResolution[nativeResolutionString]
                : fileForResolution[Config::value("fallback-resolution", "default").toString()];

        d->skin = skin;

        initialize(QUrl::fromLocalFile(skin->path() % "/" % urlPath));
    }
    else {
        qWarning() << "Can't read" << skin->name();
    }
}

void Frontend::initialize(const QUrl &targetUrl)
{
    if (targetUrl.isEmpty() || !targetUrl.isValid())
        qFatal("You are explicitly forcing a broken url on the skin system");

    delete d->centralWidget;

    if (targetUrl.path().right(3) == "qml")
    {
        QDeclarativeView *centralWidget= new QDeclarativeView(this);
        centralWidget->setAutoFillBackground(false);
        QDeclarativeEngine *engine = centralWidget->engine();

        // register dataproviders to QML
        qmlRegisterType<ActionMapper>("ActionMapper", 1, 0, "ActionMapper");
        qmlRegisterType<QMHPlugin>("QMHPlugin", 1, 0, "QMHPlugin");
        qmlRegisterType<ProxyModel>("ProxyModel", 1, 0, "ProxyModel");
        qmlRegisterType<ProxyModelItem>("ProxyModel", 1, 0, "ProxyModelItem");
        qmlRegisterType<DirModel>("DirModel", 1, 0, "DirModel");
        qmlRegisterType<QMLFileWrapper>("QMLFileWrapper", 1, 0, "QMLFileWrapper");
        qmlRegisterType<Playlist>("Playlist", 1, 0, "Playlist");
        qmlRegisterType<RpcConnection>("RpcConnection", 1, 0, "RpcConnection");

        // attach global context properties
        engine->rootContext()->setContextProperty("actionmap", d->actionMap);
        engine->rootContext()->setContextProperty("mediaPlayerHelper", d->mediaPlayerHelper);
        engine->rootContext()->setContextProperty("trackpad", d->trackpad);
        engine->rootContext()->setContextProperty("frontend", this);
        engine->rootContext()->setContextProperty("skin", d->skin);
        engine->rootContext()->setContextProperty("backend", Backend::instance());
        engine->addPluginPath(Backend::instance()->resourcePath() % "/lib");
        engine->addImportPath(Backend::instance()->resourcePath() % "/imports");
        engine->addImportPath(d->skin->path());

        if (Config::isEnabled("smooth-scaling", true))
            centralWidget->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

        centralWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        centralWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        centralWidget->setFrameStyle(0);
        centralWidget->setOptimizationFlags(QGraphicsView::DontSavePainterState);
        centralWidget->scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
        centralWidget->setResizeMode(QDeclarativeView::SizeRootObjectToView);

        QObject::connect(engine, SIGNAL(quit()), qApp, SLOT(quit()));

        if (Config::isEnabled("use-gl", true))
        {
#ifdef GLVIEWPORT
            QGLWidget *viewport = new QGLWidget(centralWidget);
            viewport->setAttribute(Qt::WA_OpaquePaintEvent);
            viewport->setAttribute(Qt::WA_NoSystemBackground);
            viewport->setAutoFillBackground(false);
            centralWidget->setViewport(viewport);
#endif
            centralWidget->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        } else {
            centralWidget->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
        }
        centralWidget->rootContext()->setContextProperty("config", Config::instance());

        Backend::instance()->initialize();

        foreach (QMHPlugin *plugin, Backend::instance()->allEngines())
            plugin->registerPlugin(centralWidget->rootContext());

        {
            const QMetaObject &PluginMO = QMHPlugin::staticMetaObject;
            int enumIndex = PluginMO.indexOfEnumerator("PluginRole");
            QMetaEnum roleEnum = PluginMO.enumerator(enumIndex);

            foreach (QObject *p, Backend::instance()->advertizedEngines()) {
                QMHPlugin *plugin = qobject_cast<QMHPlugin *>(p);
                if (plugin && plugin->role() < QMHPlugin::SingletonRoles) {
                    centralWidget->rootContext()->setContextProperty(QString(roleEnum.valueToKey(plugin->role())).toLower() + "Engine", plugin);
                }
            }
        }

        resetLanguage();
        d->centralWidget = centralWidget;
        centralWidget->setSource(targetUrl);

        d->centralWidget->setAttribute(Qt::WA_OpaquePaintEvent);
        d->centralWidget->setAttribute(Qt::WA_NoSystemBackground);
    }
}

void Frontend::resetLanguage()
{
    Backend *backend = Backend::instance();
    QString language = backend->language();

    delete d->frontEndTranslator;
    d->frontEndTranslator = new QTranslator(this);
    //FIXME: this clearly needs some heuristics
    d->frontEndTranslator->load(d->skin->path() % "/confluence/translations/" % "confluence_" % language % ".qm");
    qApp->installTranslator(d->frontEndTranslator);
}

void Frontend::showFullScreen()
{
    d->attemptingFullScreen = true;

    if (d->overscanWorkAround) {
        QRect geometry = Config::value("overscan-geometry", d->defaultGeometry);
        geometry.moveCenter(qApp->desktop()->availableGeometry().center());
        setGeometry(geometry);

        setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        setWindowState(Qt::WindowNoState);

        show();
    } else {
        QWidget::showFullScreen();
    }

    activateWindow();
}

void Frontend::showNormal()
{
    d->attemptingFullScreen = false;

    setWindowFlags(Qt::Window);
    setGeometry(Config::value("window-geometry", d->defaultGeometry));
    QWidget::showNormal();

    activateWindow();
}

void Frontend::grow()
{
    if (!d->attemptingFullScreen)
        return;

    const QRect newGeometry = geometry().adjusted(-1,-1,1,1);

    const QSize desktopSize = qApp->desktop()->screenGeometry(this).size();
    if ((newGeometry.width() > desktopSize.width())
            || (newGeometry.height() > desktopSize.height())) {
        d->overscanWorkAround = false;
        showFullScreen();
    }
    else {
        setGeometry(newGeometry);
    }
}

void Frontend::shrink()
{
    if (!d->attemptingFullScreen)
        return;

    d->overscanWorkAround = true;
    setGeometry(geometry().adjusted(1,1,-1,-1));
}

void Frontend::toggleFullScreen()
{
    if (d->attemptingFullScreen) {
        Config::setValue("overscan-geometry", geometry());
        showNormal();
    }
    else {
        Config::setValue("window-geometry", geometry());
        showFullScreen();
    }
}

QObject *Frontend::focusItem() const
{
    return qgraphicsitem_cast<QGraphicsObject *>(d->centralWidget->scene()->focusItem());
}

void Frontend::applyWebViewFocusFix(QDeclarativeItem *item) // See https://bugs.webkit.org/show_bug.cgi?id=51094
{
    item->setFlag(QGraphicsItem::ItemIsFocusScope, true);
    QList<QGraphicsItem *> children = item->childItems();
    for (int i = 0; i < children.count(); i++) {
        if (QGraphicsWidget *maybeWebView = qgraphicsitem_cast<QGraphicsWidget *>(children[i])) {
            if (maybeWebView->inherits("QGraphicsWebView"))
                maybeWebView->setFocus();
        }
    }
}

QWidget *Frontend::centralWidget() const
{
    return d->centralWidget;
}

#include "frontend.moc"
