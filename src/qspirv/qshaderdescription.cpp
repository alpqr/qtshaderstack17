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

#include "qshaderdescription_p.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

QT_BEGIN_NAMESPACE

QShaderDescription::QShaderDescription()
    : d(new QShaderDescriptionPrivate)
{
}

void QShaderDescription::detach()
{
    if (d->ref.load() != 1) {
        QShaderDescriptionPrivate *newd = new QShaderDescriptionPrivate(d);
        if (!d->ref.deref())
            delete d;
        d = newd;
    }
}

QShaderDescription::QShaderDescription(const QShaderDescription &other)
{
    d = other.d;
    d->ref.ref();
}

QShaderDescription &QShaderDescription::operator=(const QShaderDescription &other)
{
    if (d != other.d) {
        other.d->ref.ref();
        if (!d->ref.deref())
            delete d;
        d = other.d;
    }
    return *this;
}

QShaderDescription::~QShaderDescription()
{
    if (!d->ref.deref())
        delete d;
}

bool QShaderDescription::isNull() const
{
    return d->doc.isNull();
}

QVector<QShaderDescription::InOutVariable> QShaderDescription::inputVariables() const
{
    return d->inVars;
}

QVector<QShaderDescription::InOutVariable> QShaderDescription::outputVariables() const
{
    return d->outVars;
}

static const QString nameKey = QLatin1String("name");
static const QString typeKey = QLatin1String("type");
static const QString locationKey = QLatin1String("location");
static const QString bindingKey = QLatin1String("binding");

QShaderDescription::InOutVariable QShaderDescriptionPrivate::makeInOutVar(const QJsonObject &obj)
{
    QShaderDescription::InOutVariable var;
    var.name = obj[nameKey].toString();
    var.type = mapType(obj[typeKey].toString());
    if (obj.contains(locationKey))
        var.location = obj[locationKey].toInt();
    if (obj.contains(bindingKey))
        var.binding = obj[bindingKey].toInt();
    return var;
}

void QShaderDescriptionPrivate::setDocument(const QJsonDocument &newDoc)
{
    Q_ASSERT(ref.load() == 1);

    inVars.clear();
    outVars.clear();

    doc = newDoc;
    if (doc.isNull())
        return;

    QJsonObject root = doc.object();

    const QString inputsKey = QLatin1String("inputs");
    if (root.contains(inputsKey)) {
        QJsonArray inputs = root[inputsKey].toArray();
        for (int i = 0; i < inputs.count(); ++i)
            inVars.append(makeInOutVar(inputs[i].toObject()));
    }

    const QString outputsKey = QLatin1String("outputs");
    if (root.contains(outputsKey)) {
        QJsonArray outputs = root[outputsKey].toArray();
        for (int i = 0; i < outputs.count(); ++i)
            outVars.append(makeInOutVar(outputs[i].toObject()));
    }
}

QShaderDescription::VarType QShaderDescriptionPrivate::mapType(const QString &t)
{
    struct Tab {
        QString k;
        QShaderDescription::VarType v;
    } tab[] = {
        { QLatin1String("float"), QShaderDescription::Float },
        { QLatin1String("vec2"), QShaderDescription::Vec2 },
        { QLatin1String("vec3"), QShaderDescription::Vec3 },
        { QLatin1String("vec4"), QShaderDescription::Vec4 },
        { QLatin1String("mat2"), QShaderDescription::Mat2 },
        { QLatin1String("mat3"), QShaderDescription::Mat3 },
        { QLatin1String("mat4"), QShaderDescription::Mat4 },
        { QLatin1String("int"), QShaderDescription::Int },
        { QLatin1String("uint"), QShaderDescription::Uint },
        { QLatin1String("bool"), QShaderDescription::Bool },
        { QLatin1String("sampler2D"), QShaderDescription::Sampler2D },
        { QLatin1String("sampler3D"), QShaderDescription::Sampler3D },
        { QLatin1String("samplerCube"), QShaderDescription::SamplerCube }
    };
    for (size_t i = 0; i < sizeof(tab) / sizeof(Tab); ++i) {
        if (tab[i].k == t)
            return tab[i].v;
    }
    return QShaderDescription::Unknown;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QShaderDescription &sd)
{
    const QShaderDescriptionPrivate *d = sd.d;
    QDebugStateSaver saver(dbg);

    if (!sd.isNull()) {
        dbg.nospace() << "QShaderDescription("
                      << "inVars " << d->inVars
                      << " outVars " << d->outVars
                      << ')';
    } else {
        dbg.nospace() << "QShaderDescription(null)";
    }

    return dbg;
}

static const char *dbgTypeStr(QShaderDescription::VarType t)
{
    switch (t) {
    case QShaderDescription::Float:
        return "float";
    case QShaderDescription::Vec2:
        return "vec2";
    case QShaderDescription::Vec3:
        return "vec3";
    case QShaderDescription::Vec4:
        return "vec4";
    case QShaderDescription::Mat2:
        return "mat2";
    case QShaderDescription::Mat3:
        return "mat3";
    case QShaderDescription::Mat4:
        return "mat4";
    case QShaderDescription::Int:
        return "int";
    case QShaderDescription::Uint:
        return "uint";
    case QShaderDescription::Bool:
        return "bool";
    case QShaderDescription::Sampler2D:
        return "sampler2D";
    case QShaderDescription::Sampler3D:
        return "sampler3D";
    case QShaderDescription::SamplerCube:
        return "samplerCube";
    default:
        return "UNKNOWN";
    }
}

QDebug operator<<(QDebug dbg, const QShaderDescription::InOutVariable &var)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "InOutVariable(" << dbgTypeStr(var.type) << ' ' << var.name;
    if (var.location >= 0)
        dbg.nospace() << " location=" << var.location;
    if (var.binding >= 0)
        dbg.nospace() << " binding=" << var.binding;
    dbg.nospace() << ')';
    return dbg;
}
#endif

QT_END_NAMESPACE
