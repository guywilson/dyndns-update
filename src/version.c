#include "version.h"

#define __BDATE__      "2021-11-16 16:27:55"
#define __BVERSION__   "1.1.007"

const char * getVersion()
{
    return __BVERSION__;
}

const char * getBuildDate()
{
    return __BDATE__;
}
