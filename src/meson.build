unity_greeter_deps = [
  dep_gtk,
  dep_adw,
  dep_gio,
  dep_gio_unix,
  dep_glib,
  dep_graphene,
  dep_json,
  dep_act,
  dep_astal4,
  dep_astalgreet,
  dep_astallogind,
  dep_wayland,
]

unity_greeter_defs = configure_file(
  input:         'unity-greeter-defs.h.in',
  output:        'unity-greeter-defs.h',
  configuration: mirror_conf,
)

unity_greeter_sources = files(
  'main.c',
  'unity-greeter.c',
  'unity-greeter-idle.c',
)

wp_dir  = dependency('wayland-protocols').get_variable(pkgconfig: 'pkgdatadir')
wlr_dir = dependency('wlr-protocols').get_variable(pkgconfig: 'pkgdatadir')

wayland_protocols = [
  ['ext-idle-notify-v1',                      wp_dir  / 'staging/ext-idle-notify/ext-idle-notify-v1.xml'],
  ['wlr-output-power-management-unstable-v1', wlr_dir / 'unstable/wlr-output-power-management-unstable-v1.xml'],
]

wayland_generated = []
foreach p : wayland_protocols
  wayland_generated += custom_target(p[0] + '-client-header',
    input: p[1], output: p[0] + '-client-protocol.h',
    command: [wayland_scanner, 'client-header', '@INPUT@', '@OUTPUT@'])
  wayland_generated += custom_target(p[0] + '-code',
    input: p[1], output: p[0] + '-protocol.c',
    command: [wayland_scanner, 'private-code', '@INPUT@', '@OUTPUT@'])
endforeach

subdir('auth')
subdir('session')
subdir('components')

unity_greeter_resources = gnome.compile_resources(
  'unity-greeter-resources',
  'unity-greeter.gresource.xml',
  c_name: 'unity_greeter',
)

executable(
  'unity-greeter',
  unity_greeter_sources,
  unity_greeter_resources,
  wayland_generated,
  dependencies:        unity_greeter_deps,
  include_directories: include_directories('.', 'auth', 'session', 'components'),
  install:             true,
  install_dir:         bindir,
)

executable(
  'unity-greeter-session',
  'unity-greeter-session.c',
  dependencies: [dep_glib, dep_gio],
  install:      true,
  install_dir:  bindir,
)
