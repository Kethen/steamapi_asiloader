#include <windows.h>
#include <psapi.h>

#include <stdio.h>

#include <sys/types.h>
#include <dirent.h>

int iSteamApps;
FARPROC fpApiAddr[700];
WCHAR szProfile[MAX_PATH];

static FILE *log_file = NULL;
#define LOG(...) { \
	if(log_file == NULL){ \
		log_file = fopen("steamapi_asiloader.log", "w"); \
	} \
	if(log_file != NULL){ \
		fprintf(log_file, __VA_ARGS__); \
		fflush(log_file); \
	} \
}

#include "steamapi_enumerate.log"

void load_asi(){
	LOG("%s: begins\n", __func__);
	DIR *cur_dir = opendir("./");
	struct dirent *entry = readdir(cur_dir);
	while(entry != NULL){
		int name_len = strlen(entry->d_name);
		if(name_len < 4){
			LOG("%s: skipping non asi file %s\n", __func__, entry->d_name);
			entry = readdir(cur_dir);
			continue;
		}
		if(strcmp(".asi", &entry->d_name[name_len - 4]) == 0){
			if(LoadLibraryA(entry->d_name) == NULL){
				LOG("%s: failed loading %s, 0x%08x\n", __func__, GetLastError());
			}else{
				LOG("%s: loaded %s\n", __func__, entry->d_name);
			}
		}else{
			LOG("%s: skipping non asi file %s\n", __func__, entry->d_name);
		}
		entry = readdir(cur_dir);
	}
	closedir(cur_dir);
}

int __attribute__((constructor)) init(){
	char file_name[1024];
	DWORD file_name_size = GetProcessImageFileNameA(GetCurrentProcess(), file_name, sizeof(file_name));
	if(file_name_size == 0){
		LOG("%s: failed fetching process file name\n", __func__);
	}
	LOG("%s: initializing on program %s\n", __func__, file_name);

	load_syms();
	load_asi();
	return 0;
}
