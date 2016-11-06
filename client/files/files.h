#ifndef __FILES__
#define __FILES__


#include "../core/client.h"

#define MAXFILEDIR 32


bool set_work_dir(void);
bool check_download_folder(void);
void list_files(char*);
void remove_file(const char*);
void rename_file(const char*);
void close_file(FILE*, const char*);
void is_present(const char*);
void ask_list(const char*);


#endif
