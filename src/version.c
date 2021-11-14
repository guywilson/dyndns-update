#include "version.h"

#define __BDATE__      "2021-11-13 21:18:44"
#define __BVERSION__   "1.0.001"

const char * getVersion()
{
    return __BVERSION__;
}

const char * getBuildDate()
{
    return __BDATE__;
}
