#ifndef _RCCOMMON
#define _RCCOMMON

#define LL_INFOS(x) std::cout << "WARN " << x << " : "
#define LL_WARNS(x) std::cout << "WARN " << x << " : "
#define LL_DEBUGS(x) std::cout << "DEBUG " << x << " : "
#define LL_ERRS(x) std::cout << "DEBUG " << x << " : " 

#define LL_ENDL "\n";

#define _CRT_SECURE_NO_WARNINGS

#if LL_WINDOWS
#include <winsock2.h>
#include "windows.h"
#endif


#include <iostream>
#include <queue>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include "time.h"

#include "lltimer.h"

#define LL_COMMON_API /**/

using namespace std;

#endif