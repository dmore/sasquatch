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

#include "globalsettings.h"

GlobalSettings::GlobalSettings(QObject *parent) :
    Settings(parent)
{
    addOptionEntry(GlobalSettings::Skin,                "confluence",   "skin",             "<name> specifies the skin");
    addOptionEntry(GlobalSettings::SkinsPath,           "",             "skinsPath",        "<path> adds path to skins search paths");
    addOptionEntry(GlobalSettings::Keymap,              "stdkeyboard",  "keymap",           "<name> specifies the keymap");
    addOptionEntry(GlobalSettings::KeymapsPath,         "",             "keymapsPath",      "<path> adds path to keymaps search paths");
    addOptionEntry(GlobalSettings::ApplicationsPath,    "",             "appsPath",         "<path> adds path to skins search paths");
    addOptionEntry(GlobalSettings::FullScreen,          "true",         "fullscreen",       "<bool> toggle fullscreen");
    addOptionEntry(GlobalSettings::OverlayMode,         "false",        "overlayMode",      "<bool> toggle overlay mode used for devices with other mediaplayers than QtMultimediaKit");
    addOptionEntry(GlobalSettings::Headless,            "false",        "headless",         "<bool> toggle running with user interface, usable for streaming server usage");
    addOptionEntry(GlobalSettings::Proxy,               "false",        "proxy",            "<bool> use a proxy for network access");
    addOptionEntry(GlobalSettings::ProxyHost,           "localhost",    "proxyHost",        "<hostname> set proxy host, only used with -proxy=true");
    addOptionEntry(GlobalSettings::ProxyPort,           "8080",         "proxyPort",        "<port> set port number for proxy usage, only used with -proxy=true");
    addOptionEntry(GlobalSettings::MultiInstance,       "false",        "multiInstance",    "<bool> allow running multiple instances");
    addOptionEntry(GlobalSettings::Mouse,               "true",         "mouse",            "<bool> toggle mouse cursor");
    addOptionEntry(GlobalSettings::MouseTimeout,        "2",            "mouseTimeout",     "<int> hide idle mouse cursor timeout in seconds");
    addOptionEntry(GlobalSettings::DrivenFPS,           "false",        "drivenFPS",        "<bool> toggle fps counter");
    addOptionEntry(GlobalSettings::StreamingAddress,    "",             "streamingAddress", "<string> specifies the streaming address");
    addOptionEntry(GlobalSettings::StreamingPort,       "1337",         "streamingPort",    "<int> specifies the streaming port");
}

bool GlobalSettings::isEnabled(GlobalSettings::Option option) const
{
    return Settings::isEnabled(m_table[option]);
}

QVariant GlobalSettings::value(GlobalSettings::Option option) const
{
    return Settings::value(m_table[option]);
}

const QString GlobalSettings::name(GlobalSettings::Option option) const
{
    return m_table[option];
}

const QString GlobalSettings::doc(GlobalSettings::Option option) const
{
    return Settings::doc(name(option));
}

void GlobalSettings::setValue(GlobalSettings::Option option, const QVariant &value)
{
    insert(m_table[option], value);
    emit valueChanged(m_table[option], value);
}

void GlobalSettings::addOptionEntry(GlobalSettings::Option option, const QVariant &value, const QString &name, const QString &doc)
{
    m_table[option] = name;
    Settings::addOptionEntry(name, value, doc);
}