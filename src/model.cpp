#include <cstdlib>
#include <cassert>
#
#include <gtk/gtk.h>
#include <map>
extern GtkListStore *liststore;
std::map<std::string, GdkPixbuf*> iconset;

static
GdkPixbuf* get_file_mime_icon(const char *path) {
	GFile        *gfile = NULL;
	GFileInfo    *gfileinfo = NULL;
	GIcon        *gicon = NULL;
	GtkIconTheme *icontheme;
	GtkIconInfo  *iconinfo = NULL;
	GdkPixbuf    *pixbuf = NULL;
	char         *iconname;
	char        **iconnames;
	
	/* do not ref or unref */
	icontheme = gtk_icon_theme_get_default();
	
	gfile = g_file_new_for_path(path);
	if (gfile) {
		gfileinfo = g_file_query_info(gfile, "*", (GFileQueryInfoFlags) 0, NULL, NULL);
		if (gfileinfo) {
			gicon = g_file_info_get_icon(gfileinfo);
			if (G_IS_OBJECT(gicon)) {
				g_object_get(G_THEMED_ICON(gicon), "name",  &iconname,  NULL);
				g_object_get(G_THEMED_ICON(gicon), "names", &iconnames, NULL);
				
				if (iconset.count(iconname)) {
					pixbuf = GDK_PIXBUF(iconset[iconname]);
				}
				else {
					iconinfo = gtk_icon_theme_choose_icon(icontheme, (const char**)iconnames, 32,
					                                      (GtkIconLookupFlags) 0);
    				pixbuf = gtk_icon_info_load_icon(iconinfo, NULL);
    				iconset[iconname] = pixbuf;
				}
			}
		}
	}

	if (iconnames)
		g_strfreev(iconnames);
	if (iconinfo)
		g_object_unref(iconinfo);
	if (gicon)
		g_object_unref(gicon);
	if (gfileinfo)
		g_object_unref(gfileinfo);
	if (gfile)
		g_object_unref(gfile);
	
	return pixbuf;
}

int model_clear() {
	gtk_list_store_clear(liststore);
	
	for (auto pair : iconset) {
		g_object_unref(pair.second);
	}
	iconset.clear();
	
	return 0;
}

int model_add(bool st,
              const char *name,
              const char *path)
{
	assert(name && path);
	
	get_file_mime_icon(path);	
	
	GtkTreeIter iter;
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter,
	                   0, (gboolean) TRUE,
	                   1, (gchar *) name,
	                   2, (gchar *) path,
	                   -1);
	
	return 0;
}

