# Changelog

## Unreleased

### Added
- [x]   set flood.txt.region 1
        set flood.txt.region 0
        get flood.txt.region

        Default ist automatisch 0, weil _prefs im Konstruktor mit memset(&_prefs, 0, sizeof(_prefs)); initialisiert wird.
- [ ] cli_commands.md ergänzen

### Fixed
- [x] hop Anzahl berücksichtigen für probabilistische foward regel
