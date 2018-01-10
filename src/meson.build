
src = [
  'main.cpp',
  'directory.cpp',
  'model.cpp',
  'treeview.cpp'
]

res = gnome.compile_resources('res', 'res.gresource.xml')
src += res

executable('remux',
           sources : src,
           dependencies : deps,
           include_directories : inc,
           gui_app : true,
           install : true)

