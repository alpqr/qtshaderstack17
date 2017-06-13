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

#ifndef QSHADERDESCRIPTION_H
#define QSHADERDESCRIPTION_H

#include <QtShaderStack/qtshaderstackglobal.h>
#include <QString>
#include <QVector>

QT_BEGIN_NAMESPACE

struct QShaderDescriptionPrivate;

class Q_SHADERSTACK_EXPORT QShaderDescription
{
public:
    QShaderDescription();
    QShaderDescription(const QShaderDescription &other);
    QShaderDescription &operator=(const QShaderDescription &other);
    ~QShaderDescription();
    void detach();

    bool isNull() const;

    enum VarType {
        Unknown = 0,
        Float,
        Vec2,
        Vec3,
        Vec4,
        Mat2,
        Mat3,
        Mat4,
        Int,
        Uint,
        Bool,
        Sampler2D,
        Sampler3D,
        SamplerCube
    };

    struct InOutVariable {
        QString name;
        VarType type = Unknown;
        int location = -1;
        int binding = -1;
    };

    struct BlockVariable {
        QString name;
        VarType type = Unknown;
        int offset = 0;
    };

    struct UniformBlock {
        QString blockName;
        QString structName;
        QVector<BlockVariable> members;
    };

    struct PushConstantBlock {
        QString name;
        QVector<BlockVariable> members;
    };

    QVector<InOutVariable> inputVariables() const;
    QVector<InOutVariable> outputVariables() const;
    QVector<UniformBlock> uniformBlocks() const;
    QVector<PushConstantBlock> pushConstantBlocks() const;
    QVector<InOutVariable> combinedImageSamplers() const;

private:
    QShaderDescriptionPrivate *d;
    friend struct QShaderDescriptionPrivate;
#ifndef QT_NO_DEBUG_STREAM
    friend Q_SHADERSTACK_EXPORT QDebug operator<<(QDebug, const QShaderDescription &);
#endif
};

#ifndef QT_NO_DEBUG_STREAM
Q_SHADERSTACK_EXPORT QDebug operator<<(QDebug, const QShaderDescription &);
Q_SHADERSTACK_EXPORT QDebug operator<<(QDebug, const QShaderDescription::InOutVariable &);
Q_SHADERSTACK_EXPORT QDebug operator<<(QDebug, const QShaderDescription::BlockVariable &);
Q_SHADERSTACK_EXPORT QDebug operator<<(QDebug, const QShaderDescription::UniformBlock &);
Q_SHADERSTACK_EXPORT QDebug operator<<(QDebug, const QShaderDescription::PushConstantBlock &);
#endif

QT_END_NAMESPACE

#endif
