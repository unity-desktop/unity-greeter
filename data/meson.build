configure_file(
  input:         'tmpfiles.d/unity-greeter.conf.in',
  output:        'unity-greeter.conf',
  configuration: mirror_conf,
  install:       true,
  install_dir:   prefix / 'lib' / 'tmpfiles.d',
)

systemd_tmpfiles = find_program('systemd-tmpfiles', required: false)
if systemd_tmpfiles.found()
  meson.add_install_script(
    systemd_tmpfiles,
    '--create',
    prefix / 'lib' / 'tmpfiles.d' / 'unity-greeter.conf',
    skip_if_destdir: true,
  )
endif

sysconfdir = get_option('sysconfdir')

install_data(
  'greetd/config.toml',
  install_dir: sysconfdir / 'greetd',
)

install_data(
  'greetd/greetd.dconf-profile',
  install_dir: sysconfdir / 'dconf' / 'profile',
  rename: 'greetd',
)

install_data(
  'greetd/greeter-defaults.ini',
  install_dir: sysconfdir / 'dconf' / 'db' / 'greetd.d',
  rename: '00-unity-greeter',
)

install_data(
  'greetd/10-unity-greeter.rules',
  install_dir: datadir / 'polkit-1' / 'rules.d',
)

install_data(
  'greetd/override.conf',
  install_dir: sysconfdir / 'systemd' / 'system' / 'greetd.service.d',
)
