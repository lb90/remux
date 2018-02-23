#include "win.h"
#include <cstdlib>
#include <cstdint>
#include <string>
#include <windows.h>
#include <shlobj.h>
#include <glib.h>
#include <glib/gstdio.h>
#include "glibutil.h"

int get_appdata_folder(std::string& appdata_folder) {
    HRESULT  code = S_OK;
	GError  *errspec = NULL;
	int      ret = 0;
	int16_t *appdata_u16 = NULL;
	char    *appdata_u8 = NULL;
	
	code = SHGetKnownFolderPath(FOLDERID_RoamingAppData,
	                            0, NULL,
	                            (PWSTR*) &appdata_u16);
	if (FAILED(code) || (appdata_u16 == NULL)) {
		ret = -1;
		goto cleanup;
	}
	appdata_u8 = g_utf16_to_utf8((const gunichar2*) appdata_u16, -1, NULL, NULL, &errspec);
	if (appdata_u8 == NULL) {
		ret = -1;
		goto cleanup;
	}
	appdata_folder = appdata_u8;

cleanup:
	if (appdata)
		g_free(appdata);
	if (appdata_u16 != NULL)
		CoTaskMemFree(appdata_u16);

    return ret;
}

int get_config_folder(std::string& configfolder) {
	std::string appdata;

	get_appdata_folder(appdata);
	configfolder = util_build_filename(appdata, "Remux");
	if (!g_file_test(configfolder.c_str(), G_FILE_TEST_EXISTS))
		g_mkdir(configfolder.c_str(), 0);
	
	return ret;
}

void prevent_standby() {
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED);
}

void resume_standby() {
    SetThreadExecutionState(ES_CONTINUOUS);
}


