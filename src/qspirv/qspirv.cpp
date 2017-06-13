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

#include "qspirv.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "spirv_glsl.hpp"
#include "spirv_hlsl.hpp"
#include "spirv_msl.hpp"

QT_BEGIN_NAMESPACE

struct QSpirvPrivate
{
    QSpirvPrivate(QIODevice *device);
    ~QSpirvPrivate();

    void processResources();
    QJsonObject inOutObject(const spirv_cross::Resource &r);
    void addDeco(QJsonObject *obj, const spirv_cross::Resource &r);

    QByteArray ir;
    QJsonDocument doc;

    spirv_cross::CompilerGLSL *glslGen = nullptr;
    spirv_cross::CompilerHLSL *hlslGen = nullptr;
    spirv_cross::CompilerMSL *mslGen = nullptr;
};

QSpirv::QSpirv(const QString &filename)
{
    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning("QSpirv: Failed to open %s", qPrintable(filename));
        return;
    }
    d = new QSpirvPrivate(&f);
}

QSpirv::QSpirv(QIODevice *device)
    : d(new QSpirvPrivate(device))
{
}

QSpirv::~QSpirv()
{
    delete d;
}

QSpirvPrivate::QSpirvPrivate(QIODevice *device)
{
    ir = device->readAll();

    // ### little endian only for now
    glslGen = new spirv_cross::CompilerGLSL(reinterpret_cast<const uint32_t *>(ir.constData()), ir.size() / 4);

    processResources();
}

QSpirvPrivate::~QSpirvPrivate()
{
    delete mslGen;
    delete hlslGen;
    delete glslGen;
}

static QString matTypeStr(const spirv_cross::SPIRType &t)
{
    // ### ignore non-square for now
    switch (t.vecsize) {
    case 2:
        return QLatin1String("mat2");
    case 3:
        return QLatin1String("mat3");
    case 4:
        return QLatin1String("mat4");
    default:
        return QString();
    }
}

static QString vecTypeStr(const spirv_cross::SPIRType &t)
{
    switch (t.vecsize) {
    case 1:
        return QLatin1String("float");
    case 2:
        return QLatin1String("vec2");
    case 3:
        return QLatin1String("vec3");
    case 4:
        return QLatin1String("vec4");
    default:
        return QString();
    }
}

static QString imageStr(const spirv_cross::SPIRType &t)
{
    switch (t.image.dim) {
    case spv::Dim2D:
        return QLatin1String("sampler2D");
    case spv::Dim3D:
        return QLatin1String("sampler3D");
    case spv::DimCube:
        return QLatin1String("samplerCube");
    default:
        return QString();
    }
}

static QString typeStr(const spirv_cross::SPIRType &t)
{
    // ### needs a lot more than this
    QString s;
    switch (t.basetype) {
    case spirv_cross::SPIRType::Float:
        s = t.columns > 1 ? matTypeStr(t) : vecTypeStr(t);
        break;
    case spirv_cross::SPIRType::UInt:
        s = QLatin1String("uint");
        break;
    case spirv_cross::SPIRType::Int:
        s = QLatin1String("int");
        break;
    case spirv_cross::SPIRType::Boolean:
        s = QLatin1String("bool");
        break;
    case spirv_cross::SPIRType::SampledImage:
        s = imageStr(t);
        break;
    default:
        break;
    }
    return s;
}

QJsonObject QSpirvPrivate::inOutObject(const spirv_cross::Resource &r)
{
    QJsonObject obj;
    obj[QLatin1String("name")] = QString::fromStdString(r.name);

    const spirv_cross::SPIRType &t = glslGen->get_type(r.base_type_id);
    obj[QLatin1String("type")] = typeStr(t);

    addDeco(&obj, r);

    return obj;
}

void QSpirvPrivate::addDeco(QJsonObject *obj, const spirv_cross::Resource &r)
{
    if (glslGen->has_decoration(r.id, spv::DecorationLocation))
        (*obj)[QLatin1String("location")] = qint64(glslGen->get_decoration(r.id, spv::DecorationLocation));
    if (glslGen->has_decoration(r.id, spv::DecorationBinding))
        (*obj)[QLatin1String("binding")] = qint64(glslGen->get_decoration(r.id, spv::DecorationBinding));

    // ### more decorations?
}

void QSpirvPrivate::processResources()
{
    spirv_cross::ShaderResources resources = glslGen->get_shader_resources();

/*
    std::vector<Resource> uniform_buffers;
    std::vector<Resource> storage_buffers;
    std::vector<Resource> stage_inputs;
    std::vector<Resource> stage_outputs;
    std::vector<Resource> subpass_inputs;
    std::vector<Resource> storage_images;
    std::vector<Resource> sampled_images;
    std::vector<Resource> atomic_counters;
    std::vector<Resource> push_constant_buffers;
    std::vector<Resource> separate_images;
    std::vector<Resource> separate_samplers;
  */

    QJsonObject root;

    QJsonArray inputs;
    for (const spirv_cross::Resource &r : resources.stage_inputs)
        inputs.append(inOutObject(r));
    if (!inputs.isEmpty())
        root[QLatin1String("inputs")] = inputs;

    QJsonArray outputs;
    for (const spirv_cross::Resource &r : resources.stage_outputs)
        outputs.append(inOutObject(r));
    if (!outputs.isEmpty())
        root[QLatin1String("outputs")] = outputs;

    QJsonArray uniformBuffers;
    for (const spirv_cross::Resource &r : resources.uniform_buffers) {
        const spirv_cross::SPIRType &t = glslGen->get_type(r.base_type_id);
        QJsonObject uniformBuffer;
        uniformBuffer[QLatin1String("blockName")] = QString::fromStdString(r.name);
        uniformBuffer[QLatin1String("structName")] = QString::fromStdString(glslGen->get_name(r.id));
        QJsonArray members;
        uint32_t idx = 0;
        for (uint32_t memberTypeId : t.member_types) {
            QJsonObject member;
            member[QLatin1String("name")] = QString::fromStdString(glslGen->get_member_name(r.base_type_id, idx));
            member[QLatin1String("type")] = typeStr(glslGen->get_type(memberTypeId));
            member[QLatin1String("offset")] = qint64(glslGen->type_struct_member_offset(t, idx));
            // ### ignores a lot of things for now (arrays, decorations)
            members.append(member);
            ++idx;
        }
        uniformBuffer[QLatin1String("members")] = members;
        uniformBuffers.append(uniformBuffer);
    }
    if (!uniformBuffers.isEmpty())
        root[QLatin1String("uniformBuffers")] = uniformBuffers;

    QJsonArray pushConstantBlocks; // maps to a plain GLSL struct regardless of version
    for (const spirv_cross::Resource &r : resources.push_constant_buffers) {
        const spirv_cross::SPIRType &t = glslGen->get_type(r.base_type_id);
        QJsonObject pushConstantBlock;
        pushConstantBlock[QLatin1String("name")] = QString::fromStdString(glslGen->get_name(r.id));
        QJsonArray members;
        uint32_t idx = 0;
        for (uint32_t memberTypeId : t.member_types) {
            QJsonObject member;
            member[QLatin1String("name")] = QString::fromStdString(glslGen->get_member_name(r.base_type_id, idx));
            member[QLatin1String("type")] = typeStr(glslGen->get_type(memberTypeId));
            members.append(member);
            ++idx;
        }
        pushConstantBlock[QLatin1String("members")] = members;
        pushConstantBlocks.append(pushConstantBlock);
    }
    if (!pushConstantBlocks.isEmpty())
        root[QLatin1String("pushConstantBlocks")] = pushConstantBlocks;

    QJsonArray combinedSamplers;
    for (const spirv_cross::Resource &r : resources.sampled_images) {
        const spirv_cross::SPIRType &t = glslGen->get_type(r.base_type_id);
        QJsonObject sampler;
        sampler[QLatin1String("name")] = QString::fromStdString(r.name);
        sampler[QLatin1String("type")] = typeStr(t);
        addDeco(&sampler, r);
        combinedSamplers.append(sampler);
    }
    if (!combinedSamplers.isEmpty())
        root[QLatin1String("combinedImageSamplers")] = combinedSamplers;

    doc = QJsonDocument(root);
}

bool QSpirv::isValid() const
{
    return d && !d->doc.isEmpty();
}

QByteArray QSpirv::reflectionBinaryJson() const
{
    return d->doc.toBinaryData();
}

QByteArray QSpirv::reflectionJson() const
{
    return d->doc.toJson();
}

QByteArray QSpirv::translateToGLSL(int version, GlslFlags flags)
{
    // create a new instance since options handling seem to be problematic
    // (won't pick up new options on the second and subsequent compile())
    delete d->glslGen;
    d->glslGen = new spirv_cross::CompilerGLSL(reinterpret_cast<const uint32_t *>(d->ir.constData()), d->ir.size() / 4);

    spirv_cross::CompilerGLSL::Options options;
    options.version = version;
    options.es = flags.testFlag(GlslEs);
    options.vertex.fixup_clipspace = flags.testFlag(FixClipSpace);
    options.fragment.default_float_precision = flags.testFlag(FragDefaultMediump)
            ? spirv_cross::CompilerGLSL::Options::Mediump
            : spirv_cross::CompilerGLSL::Options::Highp;
    d->glslGen->set_options(options);

    const std::string glsl = d->glslGen->compile();

    QByteArray src = QByteArray::fromStdString(glsl);

    // Fix it up by adding #extension GL_ARB_separate_shader_objects : require
    // as well in order to make Mesa and perhaps others happy.
    const QByteArray searchStr = QByteArrayLiteral("#extension GL_ARB_shading_language_420pack : require\n#endif\n");
    int pos = src.indexOf(searchStr);
    if (pos >= 0) {
        src.insert(pos + searchStr.count(), QByteArrayLiteral("#ifdef GL_ARB_separate_shader_objects\n"
                                                              "#extension GL_ARB_separate_shader_objects : require\n"
                                                              "#endif\n"));
    }

    return src;
}

QByteArray QSpirv::translateToHLSL()
{
    if (!d->hlslGen)
        d->hlslGen = new spirv_cross::CompilerHLSL(reinterpret_cast<const uint32_t *>(d->ir.constData()), d->ir.size() / 4);

    const std::string hlsl = d->hlslGen->compile();

    return QByteArray::fromStdString(hlsl);
}

QByteArray QSpirv::translateToMSL()
{
    if (!d->mslGen)
        d->mslGen = new spirv_cross::CompilerMSL(reinterpret_cast<const uint32_t *>(d->ir.constData()), d->ir.size() / 4);

    const std::string msl = d->mslGen->compile();

    return QByteArray::fromStdString(msl);
}

QT_END_NAMESPACE
