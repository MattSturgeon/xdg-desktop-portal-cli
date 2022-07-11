xdg_desktop_portal_cli_sources = [
  'main.c',
  'xdg-portal.c',
  'event-loop.c',
  'command.c',
  'line-wrapper.c',
  'dynamic-launcher.c',
]

xdg_desktop_portal_cli_deps = [
  dependency('glib-2.0'),
  dependency('gio-2.0'),
  dependency('libportal', version: '>= 0.6'),
]

executable('xdg-portal', xdg_desktop_portal_cli_sources,
  dependencies: xdg_desktop_portal_cli_deps,
  install: true,
)
