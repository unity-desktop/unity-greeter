unity_greeter_deps = [
  dep_gtk,
  dep_adw,
  dep_gio,
  dep_gio_unix,
  dep_glib,
  dep_graphene,
  dep_json,
  dep_act,
  dep_astalgreet,
  dep_libdex,
  dep_unity_platform_quit,
  dep_astalwl,
  dep_astalidle,
  dep_unity_platform_wlr,
  dep_pwquality,
]

unity_greeter_sources = files(
  'main.c',
  'unity-greeter.c',
  'unity-greeter-idle.c',
)

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
  dependencies:        unity_greeter_deps,
  include_directories: include_directories('.', 'auth', 'session', 'components'),
  install:             true,
)

executable(
  'unity-greeter-session',
  'unity-greeter-session.c',
  dependencies: [dep_glib, dep_gio],
  install:      true,
)
