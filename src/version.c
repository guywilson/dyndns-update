#include "version.h"

#define __BDATE__      "2021-11-14 18:05:27"
#define __BVERSION__   "1.0.004"

const char * getVersion()
{
    return __BVERSION__;
}

const char * getBuildDate()
{
    return __BDATE__;
}
