unity_greeter_sources += files(
  'unity-greeter-publish.c',
)

executable(
  'unity-greeter-publisher',
  'main.c',
  'unity-greeter-outputs.c',
  'unity-greeter-publish.c',
  'unity-greeter-wallpaper.c',
  dependencies: [dep_gtk, dep_unity_platform_backgrounds],
  install:      true,
)
