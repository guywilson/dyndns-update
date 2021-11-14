#include "version.h"

#define __BDATE__      "2021-11-14 14:26:30"
#define __BVERSION__   "1.0.002"

const char * getVersion()
{
    return __BVERSION__;
}

const char * getBuildDate()
{
    return __BDATE__;
}
