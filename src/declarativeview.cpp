/****************************************************************************

This file is part of the QtMediaHub project on http://www.qtmediahub.com

Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).*
All rights reserved.

Contact:  Girish Ramakrishnan girish@forwardbias.in
Contact:  Nokia Corporation donald.carr@nokia.com
Contact:  Nokia Corporation johannes.zellner@nokia.com

You may use this file under the terms of the BSD license as follows:

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of Nokia Corporation and its Subsidiary(-ies) nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

****************************************************************************/

#include "declarativeview.h"
#include <QGraphicsObject>
#include <QDeclarativeEngine>
#include <QDebug>
#include <QPaintEvent>
#include "qmh-config.h"

#ifdef GL
#include <QGLWidget>
#endif

DeclarativeView::DeclarativeView(QWidget *parent)
    : QDeclarativeView(parent),
      m_frameCount(0),
      m_fps(0)
{
    startTimer(1000);
    connect(this, SIGNAL(statusChanged(QDeclarativeView::Status)), this, SLOT(handleStatusChanged(QDeclarativeView::Status)));
}

void DeclarativeView::setSource(const QUrl &url)
{
    m_url = url;
    QMetaObject::invokeMethod(this, "handleSourceChanged", Qt::QueuedConnection);
}

void DeclarativeView::paintEvent(QPaintEvent *event)
{
    static bool isGLViewport
#ifdef GL
            = qobject_cast<QGLWidget*>(viewport());
#else
            = false;
#endif
    if (!isGLViewport) {
        bool isOverlayMode = Config::isEnabled("overlay-mode", false);
        if (isOverlayMode) {
            QPainter painter(viewport());
            painter.fillRect(event->rect(), Qt::transparent);
        }
    }
    QDeclarativeView::paintEvent(event);
    m_timeSigma += m_frameTimer.elapsed();
    ++m_frameCount;
    m_frameTimer.restart();
}

void DeclarativeView::timerEvent(QTimerEvent *event)
{
    if (m_timeSigma) {
        m_fps = 1000*m_frameCount/m_timeSigma;
        m_timeSigma = m_frameCount = 0;
        emit fpsChanged();
    }
    QDeclarativeView::timerEvent(event);
}

void DeclarativeView::handleSourceChanged()
{
    QDeclarativeView::setSource(m_url);
}

void DeclarativeView::handleStatusChanged(QDeclarativeView::Status status)
{
    if (status == QDeclarativeView::Ready) {
        activateWindow();
    }
}

QObject *DeclarativeView::focusItem() const
{
    return qgraphicsitem_cast<QGraphicsObject *>(scene()->focusItem());
}

int DeclarativeView::fps() const
{
    return m_fps;
}

void DeclarativeView::addImportPath(const QString &path)
{
    engine()->addImportPath(path);
}

