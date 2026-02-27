expand_content_md_files = [
  'about-wad.md',
]

if get_option('documentation')
  wad_toml = configure_file(
    input: 'wad.toml.in',
    output: 'wad.toml',
    configuration: toml_conf,
    install: true,
    install_dir: docs_dir / 'wad',
  )

  custom_target(
    'wad-doc',
    input: [wad_toml, wad_gir[0]],
    output: 'wad',
    command: [
      gidocgen,
      'generate',
      gidocgen_common_args,
      '--add-include-path=@0@'.format(meson.current_build_dir() / '../../wad'),
      '--config=@INPUT0@',
      '--output-dir=@OUTPUT@',
      '--content-dir=@0@'.format(meson.current_source_dir()),
      '@INPUT1@',
    ],
    depend_files: [expand_content_md_files],
    build_by_default: true,
    install: true,
    install_dir: docs_dir,
  )
endif
