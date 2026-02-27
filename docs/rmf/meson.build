expand_content_md_files = [
  'about-rmf.md',
]

if get_option('documentation')
  rmf_toml = configure_file(
    input: 'rmf.toml.in',
    output: 'rmf.toml',
    configuration: toml_conf,
    install: true,
    install_dir: docs_dir / 'rmf',
  )

  custom_target(
    'rmf-doc',
    input: [rmf_toml, rmf_gir[0]],
    output: 'rmf',
    command: [
      gidocgen,
      'generate',
      gidocgen_common_args,
      '--add-include-path=@0@'.format(meson.current_build_dir() / '../../rmf'),
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
