
src = [
  'main.cpp',
  'app.cpp',
  'op.cpp',
  'model.cpp',
  'elements.cpp',
  'scandirectory.cpp',
  'mediascan.cpp',
  'parseinfo.cpp',
  'mediaconvert.cpp',
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

if host_machine.system() == 'windows'
  src += 'launchprocess_win32.cpp'
else
  src += 'launchprocess_glib.cpp'
endif

res = gnome.compile_resources('res', 'res.gresource.xml')
src += res

gui = false
if get_option('buildtype') == 'release'
  gui = true

executable('remux',
           sources : src,
           dependencies : deps,
           include_directories : inc,
           gui_app : gui,
           install : true)

