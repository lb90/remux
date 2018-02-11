#include "win.h"
#include <cstdlib>
#include <cstdint>
#include <string>
#include <windows.h>
#include <shlobj.h>
#include <glib.h>
#include "glibutil.h"

int get_config_folder() {
	HRESULT  code = S_OK;
	int      ret = 0;
	int16_t *appdata_u16 = NULL;
	char    *appdata = NULL;
	std::string appdata_remux;
	
	code = SHGetKnownFolderPathW(FOLDERID_RoamingAppData,
	                             0, NULL,
	                             (PWSTR*) &appdata_u16);
	if (FAILED(hr) || (appdata_u16 == NULL)) {
		ret = -1;
		//if (FAILED(hr))
		goto cleanup;
	}
	appdata = g_utf16_to_utf8(appdata_u16, -1, NULL, NULL, &errspec);
	if (appdata == NULL) {
		ret = -1;
		goto cleanup;
	}
	appdata_remux = util_build_filename(appdata, "Remux");
	if (!g_file_test(appdata_remux, G_FILE_TEST_EXISTS))
		g_mkdir(appdata_remux.c_str(), 0);

cleanup:
	if (appdata)
		g_free(appdata);
	if (appdata_u16 != NULL)
		CoTaskMemFree(appdata_u16);
	
	return ret;
}

