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

#include "qshader.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QHash>

QT_BEGIN_NAMESPACE

struct QShaderPrivate
{
    QShaderPrivate(const QString &filenamePrefix);

    QByteArray loadBlob(const char *suffix, bool text = false);

    QString prefix;

    bool triedSpirv = false;
    QByteArray spirv;

    bool triedGlsl = false;
    QVector<int> glslVersions;
    QHash<int, QByteArray> glsl;

    bool triedHlsl = false;
    QByteArray hlsl;

    bool triedMsl = false;
    QByteArray msl;
};

QShaderPrivate::QShaderPrivate(const QString &filenamePrefix)
    : prefix(filenamePrefix)
{
    /*
     .spv
     .refl
     .glslNNN
     .hlsl
     .msl
    */
}

QByteArray QShaderPrivate::loadBlob(const char *suffix, bool text)
{
    QFile f(prefix + QString::fromUtf8(suffix));
    QIODevice::OpenMode openMode = QIODevice::ReadOnly;
    if (text)
        openMode |= QIODevice::Text;
    if (!f.open(openMode))
        return QByteArray();
    return f.readAll();
}

QShader::QShader(const QString &filenamePrefix)
    : d(new QShaderPrivate(filenamePrefix))
{
}

QShader::~QShader()
{
    delete d;
}

QByteArray QShader::spirv()
{
    if (!d->triedSpirv) {
        d->triedSpirv = true;
        d->spirv = d->loadBlob(".spv");
    }
    return d->spirv;
}

QByteArray QShader::glsl(const QSurfaceFormat &format)
{
    // ### mapping to 100 is not always correct, shaders using gl_PointCoord need 120 for instance
    return glsl(format.profile() == QSurfaceFormat::CoreProfile ? 150 : 100);
}

QVector<int> QShader::availableGlslVersions()
{
    if (!d->triedGlsl) {
        d->triedGlsl = true;
        QFileInfo fi(d->prefix);
        QDir dir(fi.canonicalPath());
        QStringList files = dir.entryList({ fi.fileName() + QLatin1String(".glsl*") });
        for (const QString &fn : files) {
            bool ok = false;
            int v = fn.right(3).toInt(&ok);
            if (ok)
                d->glslVersions.append(v);
        }
    }
    return d->glslVersions;
}

QByteArray QShader::glsl(int version)
{
    if (!availableGlslVersions().contains(version))
        return QByteArray();

    if (!d->glsl.contains(version)) {
        QByteArray suffix = QByteArrayLiteral(".glsl");
        suffix.append(QByteArray::number(version));
        d->glsl[version] = d->loadBlob(suffix.constData(), true);
    }

    return d->glsl[version];
}

QByteArray QShader::hlsl()
{
    if (!d->triedHlsl) {
        d->triedHlsl = true;
        d->hlsl = d->loadBlob(".hlsl", true);
    }
    return d->hlsl;
}

QByteArray QShader::msl()
{
    if (!d->triedMsl) {
        d->triedMsl = true;
        d->msl = d->loadBlob(".msl", true);
    }
    return d->msl;
}

QT_END_NAMESPACE
