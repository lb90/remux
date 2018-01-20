
src = [
  'main.cpp',
  'scandirectory.cpp',
  'model.cpp',
  'window.cpp',
  'app.cpp',
  'treeview.cpp',
  'parseinfo.cpp'
# 'gtkmodel.cpp'
]

res = gnome.compile_resources('res', 'res.gresource.xml')
src += res

executable('remux',
           sources : src,
           dependencies : deps,
           include_directories : inc,
           gui_app : true,
           install : true)

