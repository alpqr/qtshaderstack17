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

#include "qspirvshader.h"
#include "qshaderdescription_p.h"
#include <QFile>

#define SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS
#include "spirv_glsl.hpp"
#include "spirv_hlsl.hpp"
#include "spirv_msl.hpp"

QT_BEGIN_NAMESPACE

struct QSpirvShaderPrivate
{
    ~QSpirvShaderPrivate();

    void createGLSLCompiler();
    void processResources();

    void addDeco(QShaderDescription::InOutVariable *v, const spirv_cross::Resource &r);
    QShaderDescription::InOutVariable inOutVar(const spirv_cross::Resource &r);

    QByteArray ir;
    QShaderDescription shaderDescription;

    spirv_cross::CompilerGLSL *glslGen = nullptr;
    spirv_cross::CompilerHLSL *hlslGen = nullptr;
    spirv_cross::CompilerMSL *mslGen = nullptr;
};

QSpirvShaderPrivate::~QSpirvShaderPrivate()
{
    delete mslGen;
    delete hlslGen;
    delete glslGen;
}

void QSpirvShaderPrivate::createGLSLCompiler()
{
    delete glslGen;
    // ### little endian only for now
    glslGen = new spirv_cross::CompilerGLSL(reinterpret_cast<const uint32_t *>(ir.constData()), ir.size() / 4);
}

static QShaderDescription::VarType matVarType(const spirv_cross::SPIRType &t)
{
    // ### ignore non-square for now
    switch (t.vecsize) {
    case 2:
        return QShaderDescription::Mat2;
    case 3:
        return QShaderDescription::Mat3;
    case 4:
        return QShaderDescription::Mat4;
    default:
        return QShaderDescription::Unknown;
    }
}

static QShaderDescription::VarType vecVarType(const spirv_cross::SPIRType &t)
{
    switch (t.vecsize) {
    case 1:
        return QShaderDescription::Float;
    case 2:
        return QShaderDescription::Vec2;
    case 3:
        return QShaderDescription::Vec3;
    case 4:
        return QShaderDescription::Vec4;
    default:
        return QShaderDescription::Unknown;
    }
}

static QShaderDescription::VarType imageVarType(const spirv_cross::SPIRType &t)
{
    switch (t.image.dim) {
    case spv::Dim2D:
        return QShaderDescription::Sampler2D;
    case spv::Dim3D:
        return QShaderDescription::Sampler3D;
    case spv::DimCube:
        return QShaderDescription::SamplerCube;
    default:
        return QShaderDescription::Unknown;
    }
}

static QShaderDescription::VarType varType(const spirv_cross::SPIRType &t)
{
    // ### needs a lot more than this
    QShaderDescription::VarType vt = QShaderDescription::Unknown;
    switch (t.basetype) {
    case spirv_cross::SPIRType::Float:
        vt = t.columns > 1 ? matVarType(t) : vecVarType(t);
        break;
    case spirv_cross::SPIRType::UInt:
        vt = QShaderDescription::Uint;
        break;
    case spirv_cross::SPIRType::Int:
        vt = QShaderDescription::Int;
        break;
    case spirv_cross::SPIRType::Boolean:
        vt = QShaderDescription::Bool;
        break;
    case spirv_cross::SPIRType::SampledImage:
        vt = imageVarType(t);
        break;
    default:
        break;
    }
    return vt;
}

void QSpirvShaderPrivate::addDeco(QShaderDescription::InOutVariable *v, const spirv_cross::Resource &r)
{
    if (glslGen->has_decoration(r.id, spv::DecorationLocation))
        v->location = glslGen->get_decoration(r.id, spv::DecorationLocation);
    if (glslGen->has_decoration(r.id, spv::DecorationBinding))
        v->binding = glslGen->get_decoration(r.id, spv::DecorationBinding);

    // ### more decorations?
}

QShaderDescription::InOutVariable QSpirvShaderPrivate::inOutVar(const spirv_cross::Resource &r)
{
    QShaderDescription::InOutVariable v;
    v.name = QString::fromStdString(r.name);
    const spirv_cross::SPIRType &t = glslGen->get_type(r.base_type_id);
    v.type = varType(t);
    addDeco(&v, r);
    return v;
}

void QSpirvShaderPrivate::processResources()
{
    shaderDescription = QShaderDescription();
    QShaderDescriptionPrivate *dd = QShaderDescriptionPrivate::get(&shaderDescription);

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

    for (const spirv_cross::Resource &r : resources.stage_inputs)
        dd->inVars.append(inOutVar(r));

    for (const spirv_cross::Resource &r : resources.stage_outputs)
        dd->outVars.append(inOutVar(r));

    // uniform blocks map to either a uniform buffer or a plain struct
    for (const spirv_cross::Resource &r : resources.uniform_buffers) {
        const spirv_cross::SPIRType &t = glslGen->get_type(r.base_type_id);
        QShaderDescription::UniformBlock block;
        block.blockName = QString::fromStdString(r.name);
        block.structName = QString::fromStdString(glslGen->get_name(r.id));
        uint32_t idx = 0;
        for (uint32_t memberTypeId : t.member_types) {
            QShaderDescription::BlockVariable v;
            v.name = QString::fromStdString(glslGen->get_member_name(r.base_type_id, idx));
            v.type = varType(glslGen->get_type(memberTypeId));
            v.offset = glslGen->type_struct_member_offset(t, idx);
            // ### ignores a lot of things for now (arrays, decorations)
            block.members.append(v);
            ++idx;
        }
        dd->uniformBlocks.append(block);
    }

    // push constant blocks map to a plain GLSL struct regardless of version
    for (const spirv_cross::Resource &r : resources.push_constant_buffers) {
        const spirv_cross::SPIRType &t = glslGen->get_type(r.base_type_id);
        QShaderDescription::PushConstantBlock block;
        block.name = QString::fromStdString(glslGen->get_name(r.id));
        uint32_t idx = 0;
        for (uint32_t memberTypeId : t.member_types) {
            QShaderDescription::BlockVariable v;
            v.name = QString::fromStdString(glslGen->get_member_name(r.base_type_id, idx));
            v.type = varType(glslGen->get_type(memberTypeId));
            block.members.append(v);
            ++idx;
        }
        dd->pushConstantBlocks.append(block);
    }

    for (const spirv_cross::Resource &r : resources.sampled_images) {
        const spirv_cross::SPIRType &t = glslGen->get_type(r.base_type_id);
        QShaderDescription::InOutVariable v;
        v.name = QString::fromStdString(r.name);
        v.type = varType(t);
        addDeco(&v, r);
        dd->combinedImageSamplers.append(v);
    }
}

QSpirvShader::QSpirvShader()
    : d(new QSpirvShaderPrivate)
{
}

QSpirvShader::~QSpirvShader()
{
    delete d;
}

void QSpirvShader::setFileName(const QString &fileName)
{
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning("QSpirvShader: Failed to open %s", qPrintable(fileName));
        return;
    }
    setDevice(&f);
}

void QSpirvShader::setDevice(QIODevice *device)
{
    d->ir = device->readAll();
    d->createGLSLCompiler();
    d->processResources();
}

void QSpirvShader::setSpirvBinary(const QByteArray &spirv)
{
    d->ir = spirv;
    d->createGLSLCompiler();
    d->processResources();
}

QShaderDescription QSpirvShader::shaderDescription() const
{
    return d->shaderDescription;
}

QByteArray QSpirvShader::translateToGLSL(int version, GlslFlags flags) const
{
    // create a new instance every time since option handling seem to be problematic
    // (won't pick up new options on the second and subsequent compile())
    d->createGLSLCompiler();

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

QByteArray QSpirvShader::translateToHLSL() const
{
    if (!d->hlslGen)
        d->hlslGen = new spirv_cross::CompilerHLSL(reinterpret_cast<const uint32_t *>(d->ir.constData()), d->ir.size() / 4);

    const std::string hlsl = d->hlslGen->compile();

    return QByteArray::fromStdString(hlsl);
}

QByteArray QSpirvShader::translateToMSL() const
{
    if (!d->mslGen)
        d->mslGen = new spirv_cross::CompilerMSL(reinterpret_cast<const uint32_t *>(d->ir.constData()), d->ir.size() / 4);

    const std::string msl = d->mslGen->compile();

    return QByteArray::fromStdString(msl);
}

QT_END_NAMESPACE
