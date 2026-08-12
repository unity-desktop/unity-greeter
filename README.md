## unity-greeter

greeter for unity-desktop using `greetd` via `astal-greet`

### screenshots

| desktop form-factor| mobile form-factor |
| ------------------ | ------------------ |
| ![user list](.github/screenshots/user-list-landscape.jpg) | ![user list-portrait](.github/screenshots/user-list-portrait.png) |
| ![login prompt](.github/screenshots/login-prompt-landscape.jpg) | ![login prompt-portrait](.github/screenshots/login-prompt-portrait.png) |

**components**

- **`UnityGreeterApp`**: application entry point and lifecycle manager.
- **`UnityGreeter`**: main greeter window that displays users and sign-in flow.
- **`UnityGreeterUser`**: AccountsService user wrapper used by the greeter UI.
- **`UnityGreeterSession`**: one available desktop session discovered from `.desktop` files.
- **`UnityGreeterConversation`**: authentication/session-start conversation bridge to greetd.

### build

install the deps:

- `gtk4` (>= 4.22)
- `libadwaita-1` (>= 1.9)
- `gio-2.0` (>= 2.76)
- `gio-unix-2.0` (>= 2.76)
- `glib-2.0` (>= 2.76)
- `graphene-1.0`
- `json-glib-1.0`
- `accountsservice` (>= 23.13.9)
- `astal-greet-0.1`
- `meson`

```sh
meson setup build
ninja -C build
sudo meson install -C build
```
