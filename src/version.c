#include "version.h"

#define __BDATE__      "2021-11-18 17:41:49"
#define __BVERSION__   "1.1.008"

const char * getVersion()
{
    return __BVERSION__;
}

const char * getBuildDate()
{
    return __BDATE__;
}
