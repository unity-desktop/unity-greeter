## unity-greeter

greeter app for unity-desktop session login.

It is built on GTK 4, Libadwaita, AccountsService, and Astal Greet.

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
