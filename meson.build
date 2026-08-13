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

dep_gtk        = dependency('gtk4',            version: '>= 4.22')
dep_adw        = dependency('libadwaita-1',    version: '>= 1.9')
dep_gio        = dependency('gio-2.0',         version: '>= 2.76')
dep_gio_unix   = dependency('gio-unix-2.0',    version: '>= 2.76')
dep_glib       = dependency('glib-2.0',        version: '>= 2.76')
dep_graphene   = dependency('graphene-1.0')
dep_json       = dependency('json-glib-1.0')
dep_act        = dependency('accountsservice', version: '>= 23.13.9')
dep_astal4      = dependency('astal-4-4.0')
dep_astalgreet  = dependency('astal-greet-0.1')
dep_astallogind = dependency('astal-logind-0.1')
dep_wayland     = dependency('wayland-client')

wayland_scanner_dep = dependency('wayland-scanner', native: true)
wayland_scanner = find_program(
  wayland_scanner_dep.get_variable(pkgconfig: 'wayland_scanner'),
  native: true,
)

gnome = import('gnome')
i18n  = import('i18n')

prefix     = get_option('prefix')
bindir     = prefix / get_option('bindir')
datadir    = prefix / get_option('datadir')
localedir  = prefix / get_option('localedir')
sysconfdir = get_option('sysconfdir')

gettext_package = meson.project_name()
podir           = meson.project_source_root() / 'po'

mirror_root = '/var/lib/unity-greeter'

mirror_conf = configuration_data()
mirror_conf.set('mirror_root', mirror_root)

add_project_arguments(
  '-DGETTEXT_PACKAGE="' + gettext_package + '"',
  '-DUNITY_GREETER_LOCALEDIR="' + localedir + '"',
  '-DUNITY_GREETER_VERSION="' + meson.project_version() + '"',
  language: 'c',
)

subdir('data')
subdir('po')
subdir('src')