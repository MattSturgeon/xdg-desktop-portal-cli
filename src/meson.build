sources = [
  'main.c',
  'xdg-portal.c',
  'event-loop.c',
  'command.c',
  'line-wrapper.c',
]

subdir('util')
subdir('dynamic-launcher')

deps = [
  dependency('glib-2.0'),
  dependency('gio-2.0'),
  dependency('libportal', version: '>= 0.6'),
]

executable('xdg-portal', sources,
  dependencies: deps,
  include_directories: include,
  install: true,
)
