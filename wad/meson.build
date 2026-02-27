wad_cargs = [
  '-DWAD_COMPILATION',
  '-DG_LOG_DOMAIN="Wad"',
]

# Sources

# List of sources that do not contain public API, and should not be
wad_private_sources = files(
)

# List of files that contain public API, and should be introspected
wad_public_sources = files(
  'wad-directoryentry.c',
  'wad-fontfile.c',
  'wad-inputstream.c',
  'wad-loaderror.c',
  'wad-miptexfile.c',
  'wad-qpicfile.c',
  'wad-root.c',
  'wad-texturearchive.c',
)

wad_private_type_headers = files(
)

wad_public_headers = files(
  'wad-directoryentry.h',
  'wad-fontfile.h',
  'wad-inputstream.h',
  'wad-loaderror.c',
  'wad-miptexfile.h',
  'wad-qpicfile.h',
  'wad-rgb.h',
  'wad-root.h',
  'wad-texturearchive.h',
  'wad.h',
)

install_headers(wad_public_headers, subdir: 'wad')

wad_sources = wad_public_sources + wad_private_sources

# Library

wad_deps = [
  gio_dep,
]

libwad = library(
  'wad',
  sources: wad_sources,
  c_args: wad_cargs,
  include_directories: wad_inc,
  dependencies: wad_deps,
  install: true,
)

# Introspection
if build_gir
  gir_args = [
    '--quiet',
    #'--doc-format=gi-docgen',
    '-DWAD_COMPILATION',
  ]
  wad_gir = gnome.generate_gir(
    libwad,
    sources: [wad_public_headers, wad_public_sources],
    namespace: 'Wad',
    nsversion: goldsrc_api_version,
    identifier_prefix: 'Wad',
    symbol_prefix: 'wad',
    includes: ['GLib-2.0', 'GObject-2.0', 'Gio-2.0'],
    header: 'wad/wad.h',
    install: true,
    extra_args: gir_args,
    fatal_warnings: false,
  )
endif

libwad_dep = declare_dependency(
  include_directories: wad_inc,
  dependencies: wad_deps,
  link_with: libwad,
)

if get_option('test')
  executable('wad-test', 'test/test.c', dependencies: libwad_dep)
endif
