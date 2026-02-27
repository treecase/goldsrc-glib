project(
  'goldsrc-glib',
  'c',
  version: '0.1.0',
  default_options: [
    'c_std=c2x',
    'warning_level=2',
  ],
  meson_version: '>=1.1.0',
  license: 'GPL-3.0-or-later',
)

goldsrc_api_version = meson.project_version().split('.')[0].to_int()
goldsrc_api_name = '@0@-@1@'.format(meson.project_name(), goldsrc_api_version)

goldsrc_prefix = get_option('prefix')
goldsrc_datadir = join_paths(goldsrc_prefix, get_option('datadir'))

# Imports
gnome = import('gnome')

# Include Dirs
rmf_inc = include_directories('.')
wad_inc = include_directories('.')

# Dependencies
gio_dep = dependency('gio-2.0', version: '>=2.72')

# Build
gir = find_program('g-ir-scanner', required: get_option('introspection'), version: '>=1.80.0')
if not gir.found() and get_option('introspection').enabled()
  error('Introspection enabled, but g-ir-scanner not found.')
endif
build_gir = gir.found() and (
  get_option('introspection').enabled()
  or (get_option('introspection').allowed() and get_option('documentation'))
)

subdir('rmf')
subdir('wad')
subdir('docs')

# Summary

summary(
  {
    'documentation': get_option('documentation'),
    'introspection': build_gir,
    'test': get_option('test'),
  },
  section: 'Build',
)

summary(
  {
    'prefix': goldsrc_prefix,
    'datadir': goldsrc_datadir,
  },
  section: 'Directories',
)
