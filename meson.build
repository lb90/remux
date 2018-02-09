
project('remux', ['c', 'cpp'],
  default_options : ['cpp_std=c++14'],
  version : '1.2',
  meson_version : '>= 0.42'
)

gnome = import('gnome')

cxx = meson.get_compiler('cpp')

gtk_dep        = dependency('gtk+-3.0',
                       version: '>= 3.22')
gmodule_dep    = dependency('gmodule-2.0')
threads_dep    = dependency('threads')
boost_dep      = dependency('boost',
                       modules: [],
                       version: '>= 1.60',
                       required : false)

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

