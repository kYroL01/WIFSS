#ifndef __PROTOCOLS__
#define __PROTOCOLS__


#include "../core/client.h"

#define OK       "ok"
#define FAIL     "fail"
#define ENDT     "ENDT"
#define PRESENT  "present"
#define FINISHED "finished"

#define DOTPART ".part"


void upload(const char*);
void download(const char*);


#endif
