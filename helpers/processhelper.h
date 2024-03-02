#ifndef PROCESSHELPER_H
#define PROCESSHELPER_H

#include <QStringList>

class ProcessHelper
{
private:
    static bool _verbose;
    static const QString SEPARATOR;
    static QString _password;
public:
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

    static void setVerbose(bool v){_verbose = v;}

    static Output ShellExecute(const QString& cmd, int timeout_millis = -1);
    static void SetPassword(const QString& v){_password=v;}
    static Output ShellExecuteSudo(const QString& cmd, int timeout_millis = -1);
};

#endif // PROCESSHELPER_H
