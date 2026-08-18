tmpfiles_dir = get_option('prefix') / 'lib' / 'tmpfiles.d'

configure_file(
  input:         'tmpfiles.d/unity-greeter.conf.in',
  output:        'unity-greeter.conf',
  configuration: mirror_conf,
  install:       true,
  install_dir:   tmpfiles_dir,
)

systemd_tmpfiles = find_program('systemd-tmpfiles', required: false)
if systemd_tmpfiles.found()
  meson.add_install_script(
    systemd_tmpfiles,
    '--create',
    tmpfiles_dir / 'unity-greeter.conf',
    skip_if_destdir: true,
  )
endif

install_data(
  'greetd/config.toml',
  install_dir: get_option('sysconfdir') / 'greetd',
)

install_data(
  'greetd/greetd.dconf-profile',
  install_dir: get_option('sysconfdir') / 'dconf' / 'profile',
  rename: 'greetd',
)

install_data(
  'greetd/greeter-defaults.ini',
  install_dir: get_option('sysconfdir') / 'dconf' / 'db' / 'greetd.d',
  rename: '00-unity-greeter',
)

install_data(
  'greetd/10-unity-greeter.rules',
  install_dir: get_option('datadir') / 'polkit-1' / 'rules.d',
)
