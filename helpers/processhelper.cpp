#include "processhelper.h"
//#include "qdebug.h"
//#include "helpers/logger.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QProcess>
#include <iostream>

const QString ProcessHelper::SEPARATOR = '\n'+QStringLiteral("stderr")+'\n';
//bool ProcessHelper::_verbose = false;
//QString ProcessHelper::_password = "";
//void (*ProcessHelper::_readFn)(const QByteArray& d) = nullptr;

ProcessHelper::ProcessHelper(QObject *parent) :QObject(parent)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    env.insert("LD_LIBRARY_PATH", "/usr/lib");
    _pd.setProcessEnvironment(env);
    QString path = qApp->applicationDirPath();
    _pd.setWorkingDirectory(path);

    auto readyR2 = [this]()
    {
        _pd.setReadChannel(QProcess::StandardError);
        QByteArray d;
        while (!_pd.atEnd()) {
            d = _pd.readAll();
            if(_writeErr){
                std::cerr << d.toStdString();
            }
        }
        if(!d.endsWith('\n')) {
            if(_writeErr){
                std::cerr << QStringLiteral("\n").toStdString();
                d.append('\n');
            }
        }
        QByteArray d2(d);
        emit this->stdErrR(d2);
    };

    auto finishedR2 = [this](int exitCode, QProcess::ExitStatus exitStatus)
    {
        //ProcessHelper::Output o;
        _od.elapsedMillis = _td.elapsed();
        _od.stdOut  = _pd.readAllStandardOutput();
        _od.stdErr = _pd.readAllStandardError();
        _od.exitCode = _pd.exitCode();

        emit this->finished();
    };

    QObject::connect(&_pd, &QProcess::readyReadStandardError,readyR2);
    QObject::connect(&_pd, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),finishedR2);

}

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

    auto readyR = [&process, this]()
    {
        process.setReadChannel(QProcess::StandardError);
        QByteArray d;
        while (!process.atEnd()) {
            d = process.readAll();
            if(_writeErr){
                std::cerr << d.toStdString();
            }
        }
        if(!d.endsWith('\n')) {
            if(_writeErr){
                std::cerr << QStringLiteral("\n").toStdString();
                d.append('\n');
            }
        }
        QByteArray d2(d);
        emit this->stdErrR(d2);
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


void ProcessHelper::ShellExecuteSudoNoWait(const QString &cmd, int timeout_millis){
    if(_password.isEmpty()) return;

    QString cmd2 = QStringLiteral("echo \"%1\" | sudo -S %2").arg(_password).arg(cmd);
    return ShellExecuteNoWait(cmd2, timeout_millis);
}


void ProcessHelper::ShellExecuteNoWait(const QString &cmd, int timeout_millis)
{
    _td.restart();
    _pd.start("/bin/sh", {"-c", cmd});

    return;
}

ProcessHelper::Output ProcessHelper::GetOut()
{
    return _od;
}


// void ProcessHelper::readyR2(&process, this)()
// {
//     process.setReadChannel(QProcess::StandardError);
//     QByteArray d;
//     while (!process.atEnd()) {
//         d = process.readAll();
//         if(_writeErr){
//             std::cerr << d.toStdString();
//         }
//     }
//     if(!d.endsWith('\n')) {
//         if(_writeErr){
//             std::cerr << QStringLiteral("\n").toStdString();
//             d.append('\n');
//         }
//     }
//     QByteArray d2(d);
//     emit this->stdErrR(d2);
// };
