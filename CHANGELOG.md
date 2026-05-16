# Changelog

## Unreleased
- [ ] examples/simple_repeater/MyMesh.h anpassen:
        #define FIRMWARE_VERSION   "v1.15.0"
        #define FIRMWARE_BUILD_DATE "24-Apr-2026"

### Added
- [x]   set flood.txt.region 1
        set flood.txt.region 0
        get flood.txt.region

        Default is 0 because `_prefs` is initialized with:
        `memset(&_prefs, 0, sizeof(_prefs));`

- [ ] cli_commands.md ergänzen

### Changed

- [x] TXT_MSG can now use the same region-scope handling as GRP_TXT

### Fixed

- [x] Restored hop-count dependent probabilistic flooding
- [x] Forwarding probability now decreases with increasing hop count

## Released