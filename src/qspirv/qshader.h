/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Shader Stack module
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSHADER_H
#define QSHADER_H

#include <QtShaderStack/qtshaderstackglobal.h>
#include <QtShaderStack/qshaderdescription.h>
#include <QString>
#include <QVector>
#include <QSurfaceFormat>

QT_BEGIN_NAMESPACE

class QShaderPrivate;

class Q_SHADERSTACK_EXPORT QShader
{
public:
    QShader(const QString &filenamePrefix);
    ~QShader();

    QByteArray spirv();

    QByteArray glsl(const QSurfaceFormat &format);

    struct GlslVersion {
        GlslVersion() { }
        GlslVersion(int v, bool e) : version(v), es(e) { }
        int version = 100;
        bool es = true;
    };

    QVector<GlslVersion> availableGlslVersions();
    QByteArray glsl(const GlslVersion &version);

    QByteArray hlsl();

    QByteArray msl();

    QShaderDescription description();

private:
    Q_DISABLE_COPY(QShader)
    QShaderPrivate *d = nullptr;
};

Q_SHADERSTACK_EXPORT bool operator==(const QShader::GlslVersion &, const QShader::GlslVersion &);
Q_SHADERSTACK_EXPORT bool operator!=(const QShader::GlslVersion &, const QShader::GlslVersion &);
Q_SHADERSTACK_EXPORT uint qHash(const QShader::GlslVersion &, uint seed);

#ifndef QT_NO_DEBUG_STREAM
Q_SHADERSTACK_EXPORT QDebug operator<<(QDebug, const QShader::GlslVersion &);
#endif

QT_END_NAMESPACE

#endif
