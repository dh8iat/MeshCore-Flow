# Changelog

## Unreleased

#### Added

- Added repeater forwarding blacklists for sender prefix, RF neighbor prefix and group channel hash.
- Added CLI commands `blk.sender.*`, `blk.neighbor.*` and `blk.channel.*` using MeshCore-style `put`, `remove` and `clear` verbs.
- Added RAM-only packet statistics counters for blacklist drops: `blk_neighbor`, `blk_sender` and `blk_channel`.
- Added CLI commands `blk.stats` and `blk.stats.clear` to display and reset only the RAM-only blacklist drop counters.

#### Changed

- Replaced the earlier experimental `path.blacklist` CLI with `blk.neighbor` for clearer semantics.

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
