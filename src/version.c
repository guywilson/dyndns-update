#include "version.h"

#define __BDATE__      "2021-11-18 17:46:35"
#define __BVERSION__   "1.1.009"

const char * getVersion()
{
    return __BVERSION__;
}

const char * getBuildDate()
{
    return __BDATE__;
}
