#include "dowork.h"

//DoWork::DoWork()
//{

//}

auto DoWork::Work1(MainViewModel::DoWorkModel m) -> MainViewModel::DoWorkRModel
{
    return {QString::number(m.i+1)};
}
