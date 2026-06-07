# MeshCore-Flow – Interesting PRs and Notes

## PR1338
Status: integrated and extended

Topic:
- Probabilistic FLOOD forwarding

Integrated / adapted for MeshCore-Flow 1.16.0_1.4:
- probabilistic forwarding for:
  - RESPONSE
  - REQ
  - ANON_REQ
- configurable CLI parameters:
  - flood.response.base
  - flood.req.base

MeshCore-Flow 1.16.0_1.4 changes:
- official MeshCore 1.16.0 FLOOD, region, denyf and TRANSPORT_FLOOD behavior is kept
- ADVERT packets are not handled probabilistically and use the official flood.max.advert rule
- RESPONSE / REQ / ANON_REQ can be controlled probabilistically

Notes:
- designed to preserve:
  - DM return paths
  - ACK handling
  - PATH discovery
  - remote/admin login
- intended for dense networks and repeater environments

---

## PR1727
Status: merged

Notes:
- merged into MeshCore-Flow
- verify long-term interaction with repeater targets
- no obvious issues on Heltec V4
- possible interaction with LilyGo TLora V2.1.1.6 still under investigation

---

## PR1810
Status: merged / needs further evaluation

Notes:
- integrated into current MeshCore-Flow branch
- may influence hardware initialization or I2C behavior on some ESP32 targets
- monitor behavior on:
  - LilyGo devices
  - RTC-equipped targets
  - PMU/I2C based boards

---

# MeshCore-Flow Design Notes

## FLOOD forwarding philosophy

Goals:
- reduce unnecessary FLOOD propagation
- preserve important return/control paths
- improve stability in dense repeater networks

Key idea:
- first relay hop always allowed for values > 0.0
- probabilistic reduction starts from hop 1

Behavior:
- 0.0  = hard block
- 1.0  = always forward
- >0.0 = first hop always, then probabilistic

Recommended repeater settings:
- flood.max.advert    = 3
- flood.response.base = 0.3
- flood.req.base      = 0.3

---

# Ideas / Future Work

- Gastzugang einschränken
- blacklist für Absender von grp_txt


---

# Hardware Notes

## Heltec V4
Status:
- boots and runs correctly with current MeshCore-Flow firmware

## LilyGo TLora V2.1.1.6
Status:
- currently unstable during boot
- observed:
  - I2C errors
  - hang at "Please wait..."
- issue appears unrelated to probabilistic FLOOD logic
- further investigation required

---

# Interesting PRs
- PR2553: exponentielle Reduktion von Flood-Adverts per Hopcount. In MeshCore-Flow 1.16.0_1.4 this is covered by the official flood.max.advert rule, not by a separate flood.advert.base setting.
- PR2545: adaptiver Advert-Rate-Limiter gegen Fake-/Spam-Adverts, inkl. Stats-Befehl stats-advert-ratelimit. Für meshcore-flow fast direkt übernehmbar.
- PR1374: Repeaters sollen nicht mehr automatisch Flood-Adverts senden, sondern auf Anfrage vollständige Metadaten liefern.
- PR2341 zusammen mit PR2408: advert.max.hops     → Advertisements begrenzen, flood.path.max      → REQ/RESPONSE/PATH begrenzen
- PR2569 
