
src = [
  'main.cpp',
  'app.cpp',
  'op.cpp',
  'model.cpp',
  'scandirectory.cpp',
  'mediascan.cpp',
  'parseinfo.cpp',
  'signalcentre.cpp',
  'treeview.cpp',
  'window.cpp',
  'gtkbasicmodel.cpp',
  'dialogproperty.cpp',
  'dialogconversion.cpp',
  'dialogsettings.cpp',
  'settings.cpp',
  'glibutil.cpp'
]

res = gnome.compile_resources('res', 'res.gresource.xml')
src += res

executable('remux',
           sources : src,
           dependencies : deps,
           include_directories : inc,
           gui_app : true,
           install : true)

