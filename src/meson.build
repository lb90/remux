
src = [
  'main.cpp',
  'app.cpp',
  'op.cpp',
  'model.cpp',
  'elements.cpp',
  'scandirectory.cpp',
  'mediascan.cpp',
  'mediaconvert.cpp',
  'jsonargs.cpp',
  'signalcentre.cpp',
  'treeview.cpp',
  'window.cpp',
  'gtkbasicmodel.cpp',
  'dialogproperty.cpp',
  'dialogconversionstart.cpp',
  'dialogconversion.cpp',
  'dialogsettings.cpp',
  'settings.cpp',
  'logfile.cpp',
  'glibutil.cpp'
]

if host_machine.system() == 'windows'
  src += 'launchprocess_win32.cpp'
  src += 'win.cpp'
else
  src += 'launchprocess_glib.cpp'
endif

res = gnome.compile_resources('res', 'res.gresource.xml')
src += res

if host_machine.system() == 'windows'
  win = import('windows')
  src += win.compile_resources('winres/remux.rc')
endif

gui = false
if get_option('buildtype') == 'release'
  gui = true
endif

executable('remux',
           sources : src,
           dependencies : deps,
           include_directories : inc,
           gui_app : gui,
           install : true)

