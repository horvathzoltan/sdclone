#ifndef PROCESSHELPER_H
#define PROCESSHELPER_H

#include "qobjectdefs.h"
#include <QObject>
#include <QProcess>
#include <QStringList>

class ProcessHelper : public QObject
{
    Q_OBJECT

private:
    bool _verbose = false;
    bool _writeErr = true;

    static const QString SEPARATOR;
    QString _password="";
public:
     explicit ProcessHelper(QObject *parent = nullptr);
    struct Output{
        QString stdOut;
        QString stdErr;
        int exitCode = 1;
        QString ToString();

        qint64 elapsedMillis;

        Output(){}

        Output(int i, const QString& err){
            exitCode = i;
            stdErr = err;
        }
    };

    void setVerbose(bool v){_verbose = v;}
    void setWriteErr(bool v){_writeErr = v;}


    Output ShellExecute(const QString& cmd, int timeout_millis = -1);
    Output ShellExecuteNoWait(const QString& cmd, int timeout_millis = -1);

    void SetPassword(const QString& v){_password=v;}
    Output ShellExecuteSudo(const QString& cmd, int timeout_millis = -1);
    Output ShellExecuteSudoNoWait(const QString& cmd, int timeout_millis = -1);

    QProcess _pd;

signals:
    void stdErrR(QByteArray&d);
};

#endif // PROCESSHELPER_H
