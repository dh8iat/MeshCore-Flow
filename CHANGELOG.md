# Changelog

## dev-1-16 FLOW rebase

- Rebased FLOW features onto the `dev-1-16` branch baseline.
- Preserved upstream `dev-1-16` FLOOD, `denyf *`, regional transport-flood and `flood.max.unscoped` behaviour as the first forwarding decision layer.
- Added FLOW repeater forwarding blacklists for sender prefix, RF neighbor prefix and group channel hash.
- Added RAM-only blacklist drop counters and CLI commands `blk.stats` / `blk.stats.clear`.
- Added hop-dependent probabilistic forwarding for `ADVERT`, `RESPONSE`, `REQ` and `ANON_REQ` FLOOD payloads.
- Added CLI documentation for blacklist filtering and probabilistic FLOOD forwarding.
- MAX_NEIGHBOURS=100 für HELTEC V4
