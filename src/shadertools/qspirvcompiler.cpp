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

#include "qspirvcompiler.h"
#include <QByteArray>
#include <QFile>
#include <QFileInfo>

#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>

QT_BEGIN_NAMESPACE

static const TBuiltInResource resourceLimits = {
    /* .MaxLights = */ 32,
    /* .MaxClipPlanes = */ 6,
    /* .MaxTextureUnits = */ 32,
    /* .MaxTextureCoords = */ 32,
    /* .MaxVertexAttribs = */ 64,
    /* .MaxVertexUniformComponents = */ 4096,
    /* .MaxVaryingFloats = */ 64,
    /* .MaxVertexTextureImageUnits = */ 32,
    /* .MaxCombinedTextureImageUnits = */ 80,
    /* .MaxTextureImageUnits = */ 32,
    /* .MaxFragmentUniformComponents = */ 4096,
    /* .MaxDrawBuffers = */ 32,
    /* .MaxVertexUniformVectors = */ 128,
    /* .MaxVaryingVectors = */ 8,
    /* .MaxFragmentUniformVectors = */ 16,
    /* .MaxVertexOutputVectors = */ 16,
    /* .MaxFragmentInputVectors = */ 15,
    /* .MinProgramTexelOffset = */ -8,
    /* .MaxProgramTexelOffset = */ 7,
    /* .MaxClipDistances = */ 8,
    /* .MaxComputeWorkGroupCountX = */ 65535,
    /* .MaxComputeWorkGroupCountY = */ 65535,
    /* .MaxComputeWorkGroupCountZ = */ 65535,
    /* .MaxComputeWorkGroupSizeX = */ 1024,
    /* .MaxComputeWorkGroupSizeY = */ 1024,
    /* .MaxComputeWorkGroupSizeZ = */ 64,
    /* .MaxComputeUniformComponents = */ 1024,
    /* .MaxComputeTextureImageUnits = */ 16,
    /* .MaxComputeImageUniforms = */ 8,
    /* .MaxComputeAtomicCounters = */ 8,
    /* .MaxComputeAtomicCounterBuffers = */ 1,
    /* .MaxVaryingComponents = */ 60,
    /* .MaxVertexOutputComponents = */ 64,
    /* .MaxGeometryInputComponents = */ 64,
    /* .MaxGeometryOutputComponents = */ 128,
    /* .MaxFragmentInputComponents = */ 128,
    /* .MaxImageUnits = */ 8,
    /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
    /* .MaxCombinedShaderOutputResources = */ 8,
    /* .MaxImageSamples = */ 0,
    /* .MaxVertexImageUniforms = */ 0,
    /* .MaxTessControlImageUniforms = */ 0,
    /* .MaxTessEvaluationImageUniforms = */ 0,
    /* .MaxGeometryImageUniforms = */ 0,
    /* .MaxFragmentImageUniforms = */ 8,
    /* .MaxCombinedImageUniforms = */ 8,
    /* .MaxGeometryTextureImageUnits = */ 16,
    /* .MaxGeometryOutputVertices = */ 256,
    /* .MaxGeometryTotalOutputComponents = */ 1024,
    /* .MaxGeometryUniformComponents = */ 1024,
    /* .MaxGeometryVaryingComponents = */ 64,
    /* .MaxTessControlInputComponents = */ 128,
    /* .MaxTessControlOutputComponents = */ 128,
    /* .MaxTessControlTextureImageUnits = */ 16,
    /* .MaxTessControlUniformComponents = */ 1024,
    /* .MaxTessControlTotalOutputComponents = */ 4096,
    /* .MaxTessEvaluationInputComponents = */ 128,
    /* .MaxTessEvaluationOutputComponents = */ 128,
    /* .MaxTessEvaluationTextureImageUnits = */ 16,
    /* .MaxTessEvaluationUniformComponents = */ 1024,
    /* .MaxTessPatchComponents = */ 120,
    /* .MaxPatchVertices = */ 32,
    /* .MaxTessGenLevel = */ 64,
    /* .MaxViewports = */ 16,
    /* .MaxVertexAtomicCounters = */ 0,
    /* .MaxTessControlAtomicCounters = */ 0,
    /* .MaxTessEvaluationAtomicCounters = */ 0,
    /* .MaxGeometryAtomicCounters = */ 0,
    /* .MaxFragmentAtomicCounters = */ 8,
    /* .MaxCombinedAtomicCounters = */ 8,
    /* .MaxAtomicCounterBindings = */ 1,
    /* .MaxVertexAtomicCounterBuffers = */ 0,
    /* .MaxTessControlAtomicCounterBuffers = */ 0,
    /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
    /* .MaxGeometryAtomicCounterBuffers = */ 0,
    /* .MaxFragmentAtomicCounterBuffers = */ 1,
    /* .MaxCombinedAtomicCounterBuffers = */ 1,
    /* .MaxAtomicCounterBufferSize = */ 16384,
    /* .MaxTransformFeedbackBuffers = */ 4,
    /* .MaxTransformFeedbackInterleavedComponents = */ 64,
    /* .MaxCullDistances = */ 8,
    /* .MaxCombinedClipAndCullDistances = */ 8,
    /* .MaxSamples = */ 4,
    /* .limits = */ {
        /* .nonInductiveForLoops = */ 1,
        /* .whileLoops = */ 1,
        /* .doWhileLoops = */ 1,
        /* .generalUniformIndexing = */ 1,
        /* .generalAttributeMatrixVectorIndexing = */ 1,
        /* .generalVaryingIndexing = */ 1,
        /* .generalSamplerIndexing = */ 1,
        /* .generalVariableIndexing = */ 1,
        /* .generalConstantMatrixVectorIndexing = */ 1,
    }
};

struct QSpirvCompilerPrivate
{
    bool readFile(const QString &fn);
    bool compile();

    QString sourceFileName;
    QByteArray source;
    EShLanguage stage = EShLangVertex;
    QSpirvCompiler::Flags flags = 0;
    QByteArray spirv;
    QString log;
};

bool QSpirvCompilerPrivate::readFile(const QString &fn)
{
    QFile f(fn);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("QSpirvCompiler: Failed to open %s", qPrintable(fn));
        return false;
    }
    source = f.readAll();
    sourceFileName = fn;
    f.close();
    return true;
}

class Includer : public glslang::TShader::Includer
{
public:
    IncludeResult *includeLocal(const char *headerName,
                                const char *includerName,
                                size_t inclusionDepth) override
    {
        Q_UNUSED(headerName);
        Q_UNUSED(includerName);
        Q_UNUSED(inclusionDepth);
        return nullptr;
    }

    IncludeResult *includeSystem(const char *headerName,
                                 const char *includerName,
                                 size_t inclusionDepth) override
    {
        Q_UNUSED(headerName);
        Q_UNUSED(includerName);
        Q_UNUSED(inclusionDepth);
        return nullptr;
    }

    void releaseInclude(IncludeResult *result) override
    {
        delete result;
    }
};

class GlobalInit
{
public:
    GlobalInit() { glslang::InitializeProcess(); }
    ~GlobalInit() { glslang::FinalizeProcess(); }
};

bool QSpirvCompilerPrivate::compile()
{
    log.clear();

    if (source.isEmpty())
        return false;

    static GlobalInit globalInit;

    glslang::TShader shader(stage);
    const QByteArray fn = sourceFileName.toUtf8();
    const char *fnStr = fn.constData();
    const char *srcStr = source.constData();
    const int size = source.size();
    shader.setStringsWithLengthsAndNames(&srcStr, &size, &fnStr, 1);

    if (!flags.testFlag(QSpirvCompiler::UseOpenGLRules)) {
        shader.setEnvInput(glslang::EShSourceGlsl, stage, glslang::EShClientVulkan, 100);
        shader.setEnvClient(glslang::EShClientVulkan, 100);
        shader.setEnvTarget(glslang::EshTargetSpv, 0x00001000);
    } else {
        shader.setEnvInput(glslang::EShSourceGlsl, stage, glslang::EShClientOpenGL, 100);
        shader.setEnvClient(glslang::EShClientOpenGL, 450);
        shader.setEnvTarget(glslang::EshTargetSpv, 0x00001000);
    }

    Includer includer;
    if (!shader.parse(&resourceLimits, 100, false, EShMsgDefault, includer)) {
        qWarning("QSpirvCompiler: Failed to parse shader");
        log = QString::fromUtf8(shader.getInfoLog()).trimmed();
        return false;
    }

    glslang::TProgram program;
    program.addShader(&shader);
    if (!program.link(EShMsgDefault)) {
        qWarning("QSpirvCompiler: Link failed");
        log = QString::fromUtf8(shader.getInfoLog()).trimmed();
        return false;
    }

    std::vector<unsigned int> spv;
    glslang::GlslangToSpv(*program.getIntermediate(stage), spv);
    if (!spv.size()) {
        qWarning("Failed to generate SPIR-V");
        return false;
    }

    spirv.resize(int(spv.size() * 4));
    memcpy(spirv.data(), spv.data(), spirv.size());

    return true;
}

QSpirvCompiler::QSpirvCompiler()
    : d(new QSpirvCompilerPrivate)
{
}

QSpirvCompiler::~QSpirvCompiler()
{
    delete d;
}

void QSpirvCompiler::setSourceFile(const QString &sourceFileName)
{
    if (!d->readFile(sourceFileName))
        return;

    const QString suffix = QFileInfo(sourceFileName).suffix();
    if (suffix == QStringLiteral("vert")) {
        d->stage = EShLangVertex;
    } else if (suffix == QStringLiteral("frag")) {
        d->stage = EShLangFragment;
    } else if (suffix == QStringLiteral("tesc")) {
        d->stage = EShLangTessControl;
    } else if (suffix == QStringLiteral("tese")) {
        d->stage = EShLangTessEvaluation;
    } else if (suffix == QStringLiteral("geom")) {
        d->stage = EShLangGeometry;
    } else if (suffix == QStringLiteral("comp")) {
        d->stage = EShLangCompute;
    } else {
        qWarning("QSpirvCompiler: Unknown shader stage, defaulting to vertex");
        d->stage = EShLangVertex;
    }
}

void QSpirvCompiler::setSourceFile(const QString &sourceFileName, Stage stage)
{
    if (!d->readFile(sourceFileName))
        return;

    d->stage = EShLanguage(stage);
}

void QSpirvCompiler::setSourceString(const QByteArray &sourceString, Stage stage)
{
    d->sourceFileName = QLatin1String("<UNKNOWN>");
    d->source = sourceString;
    d->stage = EShLanguage(stage);
}

void QSpirvCompiler::setFlags(Flags flags)
{
    d->flags = flags;
}

QByteArray QSpirvCompiler::compileToSpirv()
{
    return d->compile() ? d->spirv : QByteArray();
}

QString QSpirvCompiler::errorMessage() const
{
    return d->log;
}

QT_END_NAMESPACE
