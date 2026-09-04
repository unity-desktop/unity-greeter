project(
  'unity-greeter',
  'c',
  version: '26',
  license: 'GPL-3.0-or-later',
  meson_version: '>= 1.0',
  default_options: [
    'c_std=gnu11',
    'warning_level=2',
  ],
)

dep_gtk                 = dependency('gtk4',            version: '>= 4.22')
dep_adw                 = dependency('libadwaita-1',    version: '>= 1.9')
dep_gio                 = dependency('gio-2.0',         version: '>= 2.80')
dep_gio_unix            = dependency('gio-unix-2.0',    version: '>= 2.80')
dep_glib                = dependency('glib-2.0',        version: '>= 2.80')
dep_graphene            = dependency('graphene-1.0')
dep_json                = dependency('json-glib-1.0')
dep_act                 = dependency('accountsservice', version: '>= 23.13.9')
dep_astalgreet          = dependency('astal-greet-0.1')
dep_unity_platform_quit = dependency('unity-platform-quit')
dep_astalwl             = dependency('astal-wl-0.1')
dep_astalidle           = dependency('astal-idle-notify-0.1')
dep_unity_platform_wlr  = dependency('unity-platform-wlr')
dep_pwquality   = dependency('pwquality')

gnome = import('gnome')
i18n  = import('i18n')

gettext_package = meson.project_name()

mirror_root = '/var/lib/unity-greeter'

mirror_conf = configuration_data()
mirror_conf.set('mirror_root', mirror_root)

add_project_arguments(
  '-DGETTEXT_PACKAGE="' + gettext_package + '"',
  '-DUNITY_GREETER_LOCALEDIR="' + get_option('prefix') / get_option('localedir') + '"',
  '-DUNITY_GREETER_VERSION="' + meson.project_version() + '"',
  '-DUNITY_GREETER_MIRROR_ROOT="' + mirror_root + '"',
  '-Wno-unused-parameter',
  language: 'c',
)

subdir('data')
subdir('po')
subdir('src')