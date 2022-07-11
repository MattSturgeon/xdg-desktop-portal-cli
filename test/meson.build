sources = [
  'line-wrapper.c',
  '../src/line-wrapper.c',
]

deps = [
  dependency('glib-2.0'),
]

test(
  'line-wrapper-test',
  executable('line-wrapper-test', sources, dependencies: deps),
  env: [
    'G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
    'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir()),
    'MALLOC_PERTURB_=0',
  ],
  protocol: 'tap',
)
