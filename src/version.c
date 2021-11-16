#include "version.h"

#define __BDATE__      "2021-11-16 11:55:19"
#define __BVERSION__   "1.1.006"

const char * getVersion()
{
    return __BVERSION__;
}

const char * getBuildDate()
{
    return __BDATE__;
}
