# Changelog

## Unreleased
#### Changed

#### Repeater neighbour table size increased

The maximum neighbour table size has been increased from **50** to **100** entries for the following repeater targets:

* Heltec V4
* SenseCAP Solar

This change allows repeaters in dense networks to retain information about more neighbouring nodes before older entries are replaced.

No changes were made to neighbour discovery, sorting, or reporting logic. The update only increases the available neighbour table capacity.

## Released

### v1.15.0-Flow-v1.3

#### Added

- Added repeater forwarding blacklists for sender prefix, RF neighbor prefix and group channel hash.
- Added CLI commands `blk.sender.*`, `blk.neighbor.*` and `blk.channel.*` using MeshCore-style `put`, `remove` and `clear` verbs.
- Added RAM-only packet statistics counters for blacklist drops: `blk_neighbor`, `blk_sender` and `blk_channel`.
- Added CLI commands `blk.stats` and `blk.stats.clear` to display and reset only the RAM-only blacklist drop counters.

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
