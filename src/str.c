#include <str.h>

int str_beginwith(const char *w,const char *s)
{
	while(*s) {
		if(*s != *w) {
			return 0;
		}
		w++;
		s++;
	}
	return 1;
}
