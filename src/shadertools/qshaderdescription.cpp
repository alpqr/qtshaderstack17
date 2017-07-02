/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Shader Tools module
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
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

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

bool QShaderDescription::isValid() const
{
    return !d->inVars.isEmpty() || !d->outVars.isEmpty() || !d->uniformBlocks.isEmpty() || !d->pushConstantBlocks.isEmpty();
}

QByteArray QShaderDescription::toBinaryJson() const
{
    return d->makeDoc().toBinaryData();
}

QByteArray QShaderDescription::toJson() const
{
    return d->makeDoc().toJson();
}

QShaderDescription QShaderDescription::fromBinaryJson(const QByteArray &data)
{
    QShaderDescription desc;
    QShaderDescriptionPrivate::get(&desc)->loadDoc(QJsonDocument::fromBinaryData(data));
    return desc;
}

QVector<QShaderDescription::InOutVariable> QShaderDescription::inputVariables() const
{
    return d->inVars;
}

QVector<QShaderDescription::InOutVariable> QShaderDescription::outputVariables() const
{
    return d->outVars;
}

QVector<QShaderDescription::UniformBlock> QShaderDescription::uniformBlocks() const
{
    return d->uniformBlocks;
}

QVector<QShaderDescription::PushConstantBlock> QShaderDescription::pushConstantBlocks() const
{
    return d->pushConstantBlocks;
}

QVector<QShaderDescription::InOutVariable> QShaderDescription::combinedImageSamplers() const
{
    return d->combinedImageSamplers;
}

static struct TypeTab {
    QString k;
    QShaderDescription::VarType v;
} typeTab[] = {
    // ### keep in sync with qspirvshader.cpp
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

static QString typeStr(const QShaderDescription::VarType &t)
{
    for (size_t i = 0; i < sizeof(typeTab) / sizeof(TypeTab); ++i) {
        if (typeTab[i].v == t)
            return typeTab[i].k;
    }
    return QString();
}

static QShaderDescription::VarType mapType(const QString &t)
{
    for (size_t i = 0; i < sizeof(typeTab) / sizeof(TypeTab); ++i) {
        if (typeTab[i].k == t)
            return typeTab[i].v;
    }
    return QShaderDescription::Unknown;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QShaderDescription &sd)
{
    const QShaderDescriptionPrivate *d = sd.d;
    QDebugStateSaver saver(dbg);

    if (sd.isValid()) {
        dbg.nospace() << "QShaderDescription("
                      << "inVars " << d->inVars
                      << " outVars " << d->outVars
                      << " uniformBlocks " << d->uniformBlocks
                      << " pcBlocks " << d->pushConstantBlocks
                      << " samplers " << d->combinedImageSamplers
                      << ')';
    } else {
        dbg.nospace() << "QShaderDescription(null)";
    }

    return dbg;
}

QDebug operator<<(QDebug dbg, const QShaderDescription::InOutVariable &var)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "InOutVariable(" << typeStr(var.type) << ' ' << var.name;
    if (var.location >= 0)
        dbg.nospace() << " location=" << var.location;
    if (var.binding >= 0)
        dbg.nospace() << " binding=" << var.binding;
    dbg.nospace() << ')';
    return dbg;
}

QDebug operator<<(QDebug dbg, const QShaderDescription::BlockVariable &var)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "BlockVariable(" << typeStr(var.type) << ' ' << var.name
                  << " offset=" << var.offset << ')';
    return dbg;
}

QDebug operator<<(QDebug dbg, const QShaderDescription::UniformBlock &blk)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "UniformBlock(" << blk.blockName << ' ' << blk.structName << ' ' << blk.members << ')';
    return dbg;
}

QDebug operator<<(QDebug dbg, const QShaderDescription::PushConstantBlock &blk)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "PushConstantBlock(" << blk.name << ' ' << blk.members << ')';
    return dbg;
}
#endif

static const QString nameKey = QLatin1String("name");
static const QString typeKey = QLatin1String("type");
static const QString locationKey = QLatin1String("location");
static const QString bindingKey = QLatin1String("binding");
static const QString offsetKey = QLatin1String("offset");
static const QString membersKey = QLatin1String("members");
static const QString inputsKey = QLatin1String("inputs");
static const QString outputsKey = QLatin1String("outputs");
static const QString uniformBlocksKey = QLatin1String("uniformBlocks");
static const QString blockNameKey = QLatin1String("blockName");
static const QString structNameKey = QLatin1String("structName");
static const QString pushConstantBlocksKey = QLatin1String("pushConstantBlocks");
static const QString combinedImageSamplersKey = QLatin1String("combinedImageSamplers");

static void addDeco(QJsonObject *obj, const QShaderDescription::InOutVariable &v)
{
    if (v.location >= 0)
        (*obj)[locationKey] = v.location;
    if (v.binding >= 0)
        (*obj)[bindingKey] = v.binding;

    // ### more decorations?
}

static QJsonObject inOutObject(const QShaderDescription::InOutVariable &v)
{
    QJsonObject obj;
    obj[nameKey] = v.name;
    obj[typeKey] = typeStr(v.type);
    addDeco(&obj, v);
    return obj;
}

QJsonDocument QShaderDescriptionPrivate::makeDoc()
{
    QJsonObject root;

    QJsonArray jinputs;
    for (const QShaderDescription::InOutVariable &v : qAsConst(inVars))
        jinputs.append(inOutObject(v));
    if (!jinputs.isEmpty())
        root[inputsKey] = jinputs;

    QJsonArray joutputs;
    for (const QShaderDescription::InOutVariable &v : qAsConst(outVars))
        joutputs.append(inOutObject(v));
    if (!joutputs.isEmpty())
        root[outputsKey] = joutputs;

    QJsonArray juniformBlocks;
    for (const QShaderDescription::UniformBlock &b : uniformBlocks) {
        QJsonObject juniformBlock;
        juniformBlock[blockNameKey] = b.blockName;
        juniformBlock[structNameKey] = b.structName;
        QJsonArray members;
        uint32_t idx = 0;
        for (const QShaderDescription::BlockVariable &v : b.members) {
            QJsonObject member;
            member[nameKey] = v.name;
            member[typeKey] = typeStr(v.type);
            member[offsetKey] = v.offset;
            // ### ignores a lot of things for now (arrays, decorations)
            members.append(member);
            ++idx;
        }
        juniformBlock[membersKey] = members;
        juniformBlocks.append(juniformBlock);
    }
    if (!juniformBlocks.isEmpty())
        root[uniformBlocksKey] = juniformBlocks;

    QJsonArray jpushConstantBlocks;
    for (const QShaderDescription::PushConstantBlock &b : pushConstantBlocks) {
        QJsonObject jpushConstantBlock;
        jpushConstantBlock[nameKey] = b.name;
        QJsonArray members;
        uint32_t idx = 0;
        for (const QShaderDescription::BlockVariable &v : b.members) {
            QJsonObject member;
            member[nameKey] = v.name;
            member[typeKey] = typeStr(v.type);
            members.append(member);
            ++idx;
        }
        jpushConstantBlock[membersKey] = members;
        jpushConstantBlocks.append(jpushConstantBlock);
    }
    if (!jpushConstantBlocks.isEmpty())
        root[pushConstantBlocksKey] = jpushConstantBlocks;

    QJsonArray jcombinedSamplers;
    for (const QShaderDescription::InOutVariable &v : qAsConst(combinedImageSamplers)) {
        QJsonObject sampler;
        sampler[nameKey] = v.name;
        sampler[typeKey] = typeStr(v.type);
        addDeco(&sampler, v);
        jcombinedSamplers.append(sampler);
    }
    if (!jcombinedSamplers.isEmpty())
        root[combinedImageSamplersKey] = jcombinedSamplers;

    return QJsonDocument(root);
}

static QShaderDescription::InOutVariable inOutVar(const QJsonObject &obj)
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

static QShaderDescription::BlockVariable blockVar(const QJsonObject &obj)
{
    QShaderDescription::BlockVariable var;
    var.name = obj[nameKey].toString();
    var.type = mapType(obj[typeKey].toString());
    if (obj.contains(offsetKey))
        var.offset = obj[offsetKey].toInt();
    return var;
}

void QShaderDescriptionPrivate::loadDoc(const QJsonDocument &doc)
{
    if (doc.isNull()) {
        qWarning("QShaderDescription: JSON document is empty");
        return;
    }

    Q_ASSERT(ref.load() == 1); // must be detached

    inVars.clear();
    outVars.clear();
    uniformBlocks.clear();
    pushConstantBlocks.clear();
    combinedImageSamplers.clear();

    QJsonObject root = doc.object();

    if (root.contains(inputsKey)) {
        QJsonArray inputs = root[inputsKey].toArray();
        for (int i = 0; i < inputs.count(); ++i)
            inVars.append(inOutVar(inputs[i].toObject()));
    }

    if (root.contains(outputsKey)) {
        QJsonArray outputs = root[outputsKey].toArray();
        for (int i = 0; i < outputs.count(); ++i)
            outVars.append(inOutVar(outputs[i].toObject()));
    }

    if (root.contains(uniformBlocksKey)) {
        QJsonArray ubs = root[uniformBlocksKey].toArray();
        for (int i = 0; i < ubs.count(); ++i) {
            QJsonObject ubObj = ubs[i].toObject();
            QShaderDescription::UniformBlock ub;
            ub.blockName = ubObj[blockNameKey].toString();
            ub.structName = ubObj[structNameKey].toString();
            QJsonArray members = ubObj[membersKey].toArray();
            for (const QJsonValue &member : members)
                ub.members.append(blockVar(member.toObject()));
            uniformBlocks.append(ub);
        }
    }

    if (root.contains(pushConstantBlocksKey)) {
        QJsonArray pcs = root[pushConstantBlocksKey].toArray();
        for (int i = 0; i < pcs.count(); ++i) {
            QJsonObject pcObj = pcs[i].toObject();
            QShaderDescription::PushConstantBlock pc;
            pc.name = pcObj[nameKey].toString();
            QJsonArray members = pcObj[membersKey].toArray();
            for (const QJsonValue &member : members)
                pc.members.append(blockVar(member.toObject()));
            pushConstantBlocks.append(pc);
        }
    }

    if (root.contains(combinedImageSamplersKey)) {
        QJsonArray samplers = root[combinedImageSamplersKey].toArray();
        for (int i = 0; i < samplers.count(); ++i)
            combinedImageSamplers.append(inOutVar(samplers[i].toObject()));
    }
}

QT_END_NAMESPACE
