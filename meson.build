
project('remux', ['c', 'cpp'],
  version : '1.0',
  meson_version : '>= 0.42'
)

gnome = import('gnome')

gtk_dep        = dependency('gtk+-3.0',
                       version: '>= 3.20')
gmodule_dep    = dependency('gmodule-2.0')
boost_dep      = dependency('boost',
                       modules: [],
                       version: '>= 1.60')
threads_dep    = dependency('threads')

deps = [
  gtk_dep,
  gmodule_dep,
  boost_dep,
  threads_dep
]

inc = []

config_data = configuration_data()
config_data.set_quoted('REMUX_VERSION', meson.project_version())
configure_file(output : 'config.h', configuration : config_data)
inc += include_directories('.')

subdir('src')
subdir('test')

