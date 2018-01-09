
project('remux', ['c', 'cpp'],
  version : '1.0',
  meson_version : '>= 0.42'
)

gnome = import('gnome')

boost_modules = ['system',
                 'filesystem']

gtk_dep     = dependency('gtk+-3.0',
                     version : '>= 3.20')
boost_dep   = dependency('boost',
                       modules : boost_modules,
                       version : '>= 1.60')
threads_dep = dependency('threads')

deps = [
  gtk_dep,
  boost_dep,
  threads_dep
]

inc = []
if boost_dep.version().version_compare('< 1.65')
  boostprocess_inc = include_directories('ext/boostprocess')
  inc += boostprocess_inc
endif

src = [
  'main.cpp',
  'directory.cpp',
  'model.cpp'
]

res = gnome.compile_resources('res', 'res.gresource.xml')
src += res

executable('remux',
           sources : src,
           dependencies : deps,
           include_directories : inc,
           gui_app : true,
           install : true)

