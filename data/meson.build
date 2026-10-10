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
  'greetd/10-unity-greeter.rules',
  install_dir: get_option('datadir') / 'polkit-1' / 'rules.d',
)

install_data(
  'greetd/wayfire.ini',
  install_dir: get_option('sysconfdir') / 'unity-greeter',
)

user_unit_dir = get_option('prefix') / get_option('datadir') / 'systemd' / 'user'

publisher_conf = configuration_data()
publisher_conf.set('bindir', get_option('prefix') / get_option('bindir'))

configure_file(
  input:         'systemd/unity-greeter-publisher.service.in',
  output:        'unity-greeter-publisher.service',
  configuration: publisher_conf,
  install:       true,
  install_dir:   user_unit_dir,
)

install_symlink(
  'unity-greeter-publisher.service',
  pointing_to: '..' / 'unity-greeter-publisher.service',
  install_dir: user_unit_dir / 'graphical-session.target.wants',
)
