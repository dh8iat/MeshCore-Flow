# Changelog

## Unreleased

- [ ] examples/simple_repeater/MyMesh.h anpassen:
      #define FIRMWARE_VERSION   "v1.15.0"
      #define FIRMWARE_BUILD_DATE "24-Apr-2026"

- [ ] cli_commands.md ergänzen

## Released

### v1.15.0-Flow-v1.2

#### Fixed

Fixed an issue where TXT_MSG packets without region information
could still be forwarded by repeaters even when regional flood
filtering (flood.txt.region) was enabled.

Repeaters now correctly block TXT_MSG packets that do not
contain an allowed flood region.

### v1.15.0-Flow-v1.1

#### Added

- Added CLI commands:
      set flood.txt.region 1
      set flood.txt.region 0
      get flood.txt.region

      Default is 0 because `_prefs` is initialized with:
      `memset(&_prefs, 0, sizeof(_prefs));`

#### Changed

- When regions have been assigned to DM chats using the KiekR app,
  `TXT_MSG` can now support the same region-scope limitation as `GRP_TXT`

#### Fixed

- Restored hop-count dependent probabilistic flooding
- Forwarding probability now decreases with increasing hop count
