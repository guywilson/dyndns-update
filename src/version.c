#include "version.h"

#define __BDATE__      "2021-11-14 17:24:54"
#define __BVERSION__   "1.0.003"

const char * getVersion()
{
    return __BVERSION__;
}

const char * getBuildDate()
{
    return __BDATE__;
}
