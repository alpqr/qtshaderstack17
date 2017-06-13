/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include <QtCore/qcoreapplication.h>
#include <QtCore/qcommandlineparser.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qprocess.h>
#include <QtCore/qdebug.h>
#include "qspirv.h"

static QString compile(const QString &fn)
{
    QFileInfo info(fn);
    const QString ext = info.suffix();

    QString stage;
    if (ext == QStringLiteral("vert"))
        stage = QStringLiteral("-fshader-stage=vertex");
    else if (ext == QStringLiteral("frag"))
        stage = QStringLiteral("-fshader-stage=fragment");
    else
        qWarning("Unknown file extension %s", qPrintable(ext));

    QStringList params;
    if (!stage.isEmpty())
        params << stage;
    const QString spvName = info.filePath() + QLatin1String(".spv");
    params << QLatin1String("-o") << spvName;
    params << fn;

    QString dbg = QLatin1String("glslc");
    for (const QString &p : params)
        dbg += QChar(' ') + p;
    qDebug("%s", qPrintable(dbg));

    QProcess glslc;
    QString glslcName = QLatin1String("glslc");
    QByteArray altGlslc = qgetenv("QT_GLSLC");
    if (!altGlslc.isEmpty())
        glslcName = QString::fromUtf8(altGlslc);
    glslc.start(glslcName, params);
    if (!glslc.waitForStarted()) {
        qWarning("Failed to start glslc. Set QT_GLSLC if glslc is not in PATH.");
        return QString();
    }
    if (!glslc.waitForFinished())
        return QString();

    QByteArray stdout = glslc.readAllStandardOutput();
    QByteArray stderr = glslc.readAllStandardError();
    if (!stdout.isEmpty())
        qDebug("%s", stdout.constData());
    if (!stderr.isEmpty())
        qDebug("%s", stderr.constData());

    return spvName;
}

static bool writeToFile(const QByteArray &buf, const QString &filename, bool text = false)
{
    QFile f(filename);
    QIODevice::OpenMode flags = QIODevice::WriteOnly;
    if (text)
        flags |= QIODevice::Text;
    if (!f.open(flags)) {
        qWarning("Failed to open %s for writing", qPrintable(filename));
        return false;
    }
    f.write(buf);
    return true;
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QCommandLineParser cmdLineParser;
    cmdLineParser.addHelpOption();
    cmdLineParser.addPositionalArgument(QLatin1String("file"), QObject::tr("Shader to compile. Extension must be .vert, .frag, etc."), QObject::tr("files..."));
    QCommandLineOption versionOption(QStringList() << "n" << "versions",
                                     QObject::tr("Comma-separated list of output GLSL versions (e.g. 100 es, 120, 300 es, 330, etc.). Defaults to \"100 es,120,150\". Set to \"\" to disable GLSL."),
                                     QObject::tr("version"));
    cmdLineParser.addOption(versionOption);
    QCommandLineOption clipSpaceOption(QStringList() << "c" << "fix-clipspace", QObject::tr("Fix up depth [0, w] -> [-w, w]"));
    cmdLineParser.addOption(clipSpaceOption);
    QCommandLineOption hlslOption(QStringList() << "l" << "hlsl", QObject::tr("Output HLSL as well (experimental)"));
    cmdLineParser.addOption(hlslOption);
    QCommandLineOption mslOption(QStringList() << "m" << "msl", QObject::tr("Output MSL as well (experimental)"));
    cmdLineParser.addOption(mslOption);

    cmdLineParser.process(app);

    for (const QString &fn : cmdLineParser.positionalArguments()) {
        // Compile to SPIR-V.
        const QString spvName = compile(fn);
        if (spvName.isEmpty())
            return 1;

        // Read and process the SPIR-V binary.
        QSpirv spirv(spvName);
        if (!spirv.isValid())
            return 1;

        QFileInfo info(spvName);
        const QString outBaseName = info.canonicalPath() + QChar('/') + info.completeBaseName();

        // Write out reflection info.
        const QString binReflName = outBaseName + QLatin1String(".refl");
        if (!writeToFile(spirv.reflectionBinaryJson(), binReflName))
            return 1;
        const QString textReflName = outBaseName + QLatin1String(".refl.json");
        if (!writeToFile(spirv.reflectionJson(), textReflName, true))
            return 1;

        // GLSL.
        struct GLSLVersion {
            int version = 100;
            bool es = false;
        };
        QVector<GLSLVersion> versions;
        QString versionStr = QLatin1String("100 es,120,150");
        if (cmdLineParser.isSet(versionOption))
            versionStr = cmdLineParser.value(versionOption);

        for (QString v : versionStr.split(',')) {
            v = v.trimmed();
            if (v.isEmpty())
                continue;
            GLSLVersion ver;
            ver.es = v.endsWith(QStringLiteral(" es"));
            if (ver.es)
                v = v.mid(0, v.count() - 3);
            bool ok = false;
            int val = v.toInt(&ok);
            if (ok) {
                ver.version = val;
                versions.append(ver);
            } else {
                qWarning("Invalid version %s", qPrintable(versionStr));
            }
        }

        for (const GLSLVersion &ver : versions) {
            QSpirv::GlslFlags flags = 0;
            if (ver.es)
                flags |= QSpirv::GlslEs;
            if (cmdLineParser.isSet(clipSpaceOption))
                flags |= QSpirv::FixClipSpace;
            QString glslName = outBaseName + QLatin1String(".glsl") + QString::number(ver.version);
            if (ver.es)
                glslName += QLatin1String("es");
            if (!writeToFile(spirv.translateToGLSL(ver.version, flags), glslName, true))
                return 1;
        }

        // HLSL.
        if (cmdLineParser.isSet(hlslOption)) {
            const QString hlslName = outBaseName + QLatin1String(".hlsl");
            if (!writeToFile(spirv.translateToHLSL(), hlslName, true))
                return 1;
        }

        // Metal SL.
        if (cmdLineParser.isSet(mslOption)) {
            const QString mslName = outBaseName + QLatin1String(".msl");
            if (!writeToFile(spirv.translateToMSL(), mslName, true))
                return 1;
        }
    }

    return 0;
}
