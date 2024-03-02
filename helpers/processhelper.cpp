#include "processhelper.h"
#include "qdebug.h"
//#include "helpers/logger.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QProcess>
#include <iostream>

const QString ProcessHelper::SEPARATOR = '\n'+QStringLiteral("stderr")+'\n';
bool ProcessHelper::_verbose = false;
QString ProcessHelper::_password = "";

QString ProcessHelper::Output::ToString(){
    QString e;

    if(!stdOut.isEmpty())
    {
        if(!e.isEmpty()) e+=SEPARATOR;
        e+=stdOut;
    }
    if(!stdErr.isEmpty())
    {
        if(!e.isEmpty()) e+=SEPARATOR;
        e+=stdErr;
    }
    if(!e.isEmpty()) e+=SEPARATOR;
    e+=QStringLiteral("exitCode: %1").arg(exitCode);
    return e;
}

ProcessHelper::Output ProcessHelper::ShellExecute(const QString &cmd, int timeout_millis)
{
    static QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    QProcess process;

    // process beállítása
    // workaround - https://bugreports.qt.io/browse/QTBUG-2284
    env.insert("LD_LIBRARY_PATH", "/usr/lib");
    process.setProcessEnvironment(env);
    QString path = qApp->applicationDirPath();
    process.setWorkingDirectory(path);

    // process indítása
    QElapsedTimer t;
    t.start();

    auto readyR = [&process]()
    {
        process.setReadChannel(QProcess::StandardError);
        while (!process.atEnd()) {
            QString d = process.readAll();
            std::cerr << d.toStdString();
            //o2.append(d.toStdString());
            //o2.append(d.toStdString());
        }
        std::cerr << QStringLiteral("\n").toStdString();
        //zInfo("opp");
    };

    //p->setReadChannel(QProcess::StandardError);
    QObject::connect(&process, &QProcess::readyReadStandardError,readyR);

    //process.start("/bin/sh", {"-c", cmd});

    process.start("/bin/sh", {"-c", cmd});
    if(!process.waitForStarted()) return{};
    process.waitForFinished(timeout_millis);

    QObject::disconnect(&process, &QIODevice::readyRead, nullptr, nullptr);

    ProcessHelper::Output o;
    o.elapsedMillis = t.elapsed();
    o.stdOut  = process.readAllStandardOutput();
    o.stdErr = process.readAllStandardError();
    o.exitCode = process.exitCode();

    return o;
}

ProcessHelper::Output ProcessHelper::ShellExecuteSudo(const QString &cmd, int timeout_millis){
    if(_password.isEmpty()) return Output(-1, "ProcessHelper is not inited");

    QString cmd2 = QStringLiteral("echo \"%1\" | sudo -S %2").arg(_password).arg(cmd);
    return ShellExecute(cmd2, timeout_millis);
}
