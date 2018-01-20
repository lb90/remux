
project('remux', ['c', 'cpp'],
  version : '1.0',
  meson_version : '>= 0.42'
)

gnome = import('gnome')

boost_modules = ['system',
                 'filesystem']

gtk_dep        = dependency('gtk+-3.0',
                     version : '>= 3.20')
gmodule_dep    = dependency('gmodule-2.0')
boost_dep      = dependency('boost',
                       modules : boost_modules,
                       version : '>= 1.60')
portaudio_dep  = dependency('portaudio-2.0')
ebml_dep       = dependency('libebml')
matroska_dep   = dependency('libmatroska')
threads_dep    = dependency('threads')

deps = [
  gtk_dep,
  gmodule_dep,
  boost_dep,
  portaudio_dep,
  ebml_dep,
  matroska_dep,
  threads_dep
]

inc = []
if boost_dep.version().version_compare('< 1.65')
  boostprocess_inc = include_directories('ext/boostprocess')
  inc += boostprocess_inc
endif


config_data = configuration_data()
config_data.set_quoted('REMUX_VERSION', meson.project_version())
configure_file(output : 'config.h', configuration : config_data)
inc += include_directories('.')

subdir('src')
subdir('test')

