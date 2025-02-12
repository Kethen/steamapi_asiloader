#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <dbghelp.h>

#include <vector>
#include <string>

static FILE* log_file = NULL;
#define LOG(...){ \
	if(log_file == NULL){ \
		log_file = fopen("steamapi_enumerate.log", "w"); \
	} \
	if(log_file != NULL){ \
		fprintf(log_file, __VA_ARGS__); \
	} \
}

BOOL CALLBACK EnumSymProc( 
    PSYMBOL_INFO pSymInfo,   
    ULONG SymbolSize,      
    PVOID UserContext)
{
	if(strcmp("EntryPoint", pSymInfo->Name) == 0){
		return TRUE;
	}

	std::vector<std::string> &syms = *(std::vector<std::string> *)(UserContext);
	syms.push_back(std::string(pSymInfo->Name));
	return TRUE;
}

int main(){
	char mod_name[] = "steam_api64.dll";
	HMODULE lib = LoadLibraryA(mod_name);
	if(lib == NULL){
		sprintf(mod_name, "steam_api.dll");
		lib = LoadLibraryA(mod_name);
	}

	if(lib == NULL){
		LOG("failed opening steam api dll\n");
		exit(1);
	}

	// ref https://learn.microsoft.com/en-us/windows/win32/api/dbghelp/nf-dbghelp-symenumsymbols
	HANDLE hProcess = GetCurrentProcess();
	DWORD64 BaseOfDll;
	const char *Mask = "*";
	BOOL status;

	status = SymInitialize(hProcess, NULL, FALSE);
	if(status == FALSE){
		LOG("SymInitialize failed\n");
		exit(1);
	}

	BaseOfDll = SymLoadModuleEx(hProcess, NULL, mod_name, NULL, 0, 0, NULL, 0);
	if(BaseOfDll == 0){
		LOG("SymLoadModuleEx failed\n");
		exit(1);
	}

	std::vector<std::string> syms;

	BOOL ret = SymEnumSymbols(hProcess, BaseOfDll, Mask, EnumSymProc, &syms);
	if(!ret){
		LOG("SymEnumSymbols failed\n");
		exit(1);
	}

	for(auto itr = syms.begin(); itr != syms.end(); itr++){
		LOG("static void (*%s_)() = NULL;\n", itr->c_str());
	}

	LOG("extern \"C\"{\n")
	for(auto itr = syms.begin(); itr != syms.end(); itr++){
		LOG("	void %s(void){%s_();}\n", itr->c_str(), itr->c_str());
	}
	LOG("}\n");

	LOG("static void load_syms(){"
	"	HMODULE lib = LoadLibraryA(\"valveapi.dll\");\n"
	"	if(lib == NULL){\n"
	"		lib = LoadLibraryA(\"valveapi64.dll\");\n"
	"		if(lib == NULL){\n"
	"		LOG(\"failed to load original valve api\\n\")\n"
	"			exit(1);\n"
	"		}\n"
	"	}\n"
	);

	for(auto itr = syms.begin(); itr != syms.end(); itr++){
		LOG("	%s_ = (void (*)())GetProcAddress(lib, \"%s\");\n", itr->c_str(), itr->c_str());
		LOG("	if(%s_ == NULL){\n", itr->c_str());
		LOG("		LOG(\"failed to load %s\\n\");\n", itr->c_str());
		LOG("		exit(1);\n");
		LOG("	}\n");
	}
	LOG("}")
}
