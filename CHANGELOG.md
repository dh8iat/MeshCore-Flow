# MeshCore-Flow Changelog

## 1.16.0_1.4

Based on official MeshCore v1.16.0.

### Added

- Repeater blacklist filtering:
  - `blk.sender`
  - `blk.neighbor`
  - `blk.channel`
  - `blk.stats`
  - `blk.stats.clear`
- Probabilistic forwarding for request/response FLOOD payloads:
  - `set flood.req.base <0.0..1.0>` for `REQ` and `ANON_REQ`
  - `set flood.response.base <0.0..1.0>` for `RESPONSE`

### Changed

- ADVERT packets are handled only by the official MeshCore 1.16 `flood.max.advert` rule.
- TXT_MSG no longer has special Flow handling; all FLOOD packets first follow the official MeshCore 1.16 forwarding rules.
- Flow filtering order is now:
  1. official MeshCore 1.16 region / denyf / flood.max / flood.max.advert logic
  2. Flow blacklist filtering
  3. Flow probabilistic REQ / ANON_REQ / RESPONSE filtering

### Removed

- `flood.txt.region`
- `flood.advert.base`
- `flood.anon.base`
- legacy `reserved_308` compatibility padding

### Compatibility note

The persisted `/com_prefs` layout changed. Repeater settings should be reset and configured again after flashing this version.
