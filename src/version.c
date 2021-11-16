#include "version.h"

#define __BDATE__      "2021-11-16 10:44:26"
#define __BVERSION__   "1.0.005"

const char * getVersion()
{
    return __BVERSION__;
}

const char * getBuildDate()
{
    return __BDATE__;
}
