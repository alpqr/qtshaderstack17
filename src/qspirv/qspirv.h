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

#ifndef QSPIRV_H
#define QSPIRV_H

#include <QtShaderStack/qtshaderstackglobal.h>
#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

class QIODevice;
class QSpirvPrivate;

class Q_SHADERSTACK_EXPORT QSpirv
{
public:
    enum GlslFlag {
        GlslEs = 0x01,
        FixClipSpace = 0x02,
        FragDefaultMediump = 0x04
    };
    Q_DECLARE_FLAGS(GlslFlags, GlslFlag)

    QSpirv(const QString &filename);
    QSpirv(QIODevice *device);
    ~QSpirv();

    bool isValid() const;
    QByteArray reflectionBinaryJson() const;
    QByteArray reflectionJson() const;

    QByteArray translateToGLSL(int version = 100, GlslFlags flags = GlslEs);
    QByteArray translateToHLSL();
    QByteArray translateToMSL();

private:
    Q_DISABLE_COPY(QSpirv)
    QSpirvPrivate *d = nullptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSpirv::GlslFlags)

QT_END_NAMESPACE

#endif
