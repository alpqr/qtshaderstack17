/****************************************************************************
 **
 ** Copyright (C) 2017 The Qt Company Ltd.
 ** Contact: https://www.qt.io/licensing/
 **
 ** This file is part of the test suite of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:GPL-EXCEPT$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and The Qt Company. For licensing terms
 ** and conditions see https://www.qt.io/terms-conditions. For further
 ** information use the contact form at https://www.qt.io/contact-us.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3 as published by the Free Software
 ** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
 ** included in the packaging of this file. Please review the following
 ** information to ensure the GNU General Public License requirements will
 ** be met: https://www.gnu.org/licenses/gpl-3.0.html.
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#include <QGuiApplication>
#include <QtShaderStack/QShader>
#include <QDebug>

#include "renderwindow.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QShader vs(QLatin1String(":/color.vert"));
    QShader fs(QLatin1String(":/color.frag"));

    QSurfaceFormat fmt;
    QSurfaceFormat coreFmt;
    coreFmt.setVersion(3, 3);
    coreFmt.setProfile(QSurfaceFormat::CoreProfile);

    qDebug() << "[vertex] SPIR-V size" << vs.spirv().size();
    qDebug() << "[vertex] Available variants for GLSL" << vs.availableGlslVersions();
    qDebug() << "[vertex] GLSL for default 2.0 format" << vs.glsl(vs.glslVersionForFormat(fmt));
    qDebug() << "[vertex] GLSL for 3.3 core" << vs.glsl(vs.glslVersionForFormat(coreFmt));
    QShaderDescription sd = vs.description();
    qDebug() << "[vertex] reflection data?" << !sd.isNull();
    qDebug() << "[vertex] reflection data" << sd;

    qDebug() << "\n";

    qDebug() << "[fragment] SPIR-V size" << fs.spirv().size();
    qDebug() << "[fragment] Available variants for GLSL" << fs.availableGlslVersions();
    qDebug() << "[fragment] GLSL for default 2.0 format" << fs.glsl(vs.glslVersionForFormat(fmt));
    qDebug() << "[fragment] GLSL for 3.3 core" << fs.glsl(vs.glslVersionForFormat(coreFmt));
    sd = fs.description();
    qDebug() << "[fragment] reflection data?" << !sd.isNull();
    qDebug() << "[fragment] reflection data" << sd;

    RenderWindow w(&vs, &fs, fmt);
    w.resize(1024, 768);
    w.show();

    RenderWindow cw(&vs, &fs, coreFmt);
    cw.resize(1024, 768);
    cw.show();

    return app.exec();
}
