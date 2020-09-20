# BZDB Lock

[![GitHub release](https://img.shields.io/github/release/allejo/bzdbLock.svg)](https://github.com/allejo/bzdbLock/releases/latest)
![Minimum BZFlag Version](https://img.shields.io/badge/BZFlag-v2.4.4+-blue.svg)
[![License](https://img.shields.io/github/license/allejo/bzdbLock.svg)](LICENSE.md)

A BZFlag plug-in that allows you to restrict which BZDB settings an admin is allowed or not allowed to change.

The plugin has two modes, a "whitelist" or "blacklist" mode. In whitelist mode, only the specified BZDB will allowed be to changed. In blacklist mode, every BZDB variable **except** for those listed will be allowed to be changed.

## Requirements

- C++11
- BZFS 2.4.4+

This plug-in follows [my standard instructions for compiling plug-ins](https://github.com/allejo/docs.allejo.io/wiki/BZFlag-Plug-in-Distribution).

## Usage

### Loading the plug-in

This plugin expects an INI configuration file with either the blacklist or whitelist definition specified.

```
-loadplugin bzdbLock,/path/to/variables.cfg
```

### Configuration File

The configuration is a standard INI file using either the `bzdb_blacklist` or `bzdb_whitelist` keys. [A sample configuration](./bzdbLock.cfg) is provided in this repo.

The only important part about these key/value pairs are the keys; the values are ignored and can be anything at the moment.

```ini
[bzdb_blacklist]
_worldSize=0
```

### Custom Slash Commands

| Command | Permission | Description |
| ------- | ---------- | ----------- |
| <code>/bzdblock&nbsp;[list]</code> | setAll | Lists all of the variables that can or cannot be modified depending on the mode the plugin is in. |
| <code>/set&nbsp;&lt;param&gt;</code> | setAll | Overrides the command to disable setting locked variables only. |
| <code>/reload&nbsp;[all\|bzdbLockList]</code> | setAll | Re-read the configuration file to load the BZDB lock list. |

### Inter-Plug-in Communication

This plug-in supports using generic callbacks for inter-plug-in communication. Since this plug-in uses semantic versioning in its name, accessing this plugin via a generic callback is not feasible. For this reason, the plug-in registers a clip field under the name of `allejo/bzdbLock`.

```cpp
std::string bzdbLock = bz_getclipFieldString("allejo/bzdbLock");

std::string variableName = "_worldSize";
void* data = variableName;
int response = bz_callPluginGenericCallback(bzdbLock.c_str(), "isBZDBVarLocked", data);
```

| Callback Name | Expected Type | Return Type |
| ------------- | ------------- | ----------- |
| `isBZDBVarLocked` | `std::string` | An integer representation of a boolean on whether a variable is locked. |

#### Notes

- The value of `-9999` will be returned in the case of an error

#### Warning

- Passing an object of the incorrect type will lead to unexpected behavior (and possibly server crashes?)

## License

[MIT](LICENSE.md)
