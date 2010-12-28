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

#ifndef QMLFILEWRAPPER_H
#define QMLFILEWRAPPER_H

#include <QObject>
#include <QFile>
#include <QtDeclarative>
#include <QDebug>

class QMLFileWrapper : public QObject
{
    Q_OBJECT

    public:
    Q_INVOKABLE QStringList readAll(const QString &filename)
    {
        QFile file(filename);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream textContent(&file);
        QStringList lines;
        int emptyLineCount = 0;
        while(!(file.isSequential() && textContent.atEnd())
              && (emptyLineCount < 5))
        {
            QString currentLine = textContent.readLine();
            if(currentLine.isEmpty())
                emptyLineCount++;
            else
                emptyLineCount = 0;
            lines << currentLine;
        }
        //Trim, especially important for virtual files
        for(int i = 0; i < emptyLineCount; i++)
            lines.removeLast();

        return lines;
    }

    private:
        QFile file;
};

QML_DECLARE_TYPE(QMLFileWrapper)

#endif
