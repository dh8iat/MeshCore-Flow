# MeshCore PacketTap -- Projektkonzept

## 1. Ziel des Projektes

Ziel ist der Aufbau eines Systems zur **Analyse, Dokumentation und
Visualisierung eines MeshCore-Funknetzes**.

Das System soll den kompletten Weg eines MeshCore-Funkpakets
nachvollziehbar machen -- vom Empfang über die Dekodierung bis zur
Speicherung in QuestDB und der Visualisierung in Grafana.

Der PacketTap-Receiver greift **nicht** in das MeshCore-Protokoll ein,
sondern beobachtet und dokumentiert den Funkverkehr.

------------------------------------------------------------------------

## 2. Systemübersicht

``` text
                 MeshCore Funknetz
                        │
              ┌─────────┴─────────┐
              │                   │
              ▼                   ▼
     Heltec V4 Repeater      WiFi MeshCore Node
      + PacketTap             + Companion/API
              │                   │
              ▼                   ▼
         receiver.py        mc_rx_analyzer.py
              │                   │
              └─────────┬─────────┘
                        ▼
                     QuestDB
                        │
                        ▼
                     Grafana
```

------------------------------------------------------------------------

## 3. Datenquellen des Systems

Das System soll MeshCore-Pakete aus **zwei gleichberechtigten
Datenquellen** erfassen.

### 3.1 Heltec V4 Repeater mit PacketTap

Auf einem **Heltec V4** läuft eine um PacketTap erweiterte
**MeshCore-Flow-Firmware**.

Der Heltec arbeitet weiterhin als normaler MeshCore-Repeater. PacketTap
ergänzt die Firmware lediglich um die Möglichkeit, empfangene Funkpakete
inklusive technischer Empfangsinformationen an einen Analyse-PC zu
übertragen.

PacketTap liefert unter anderem:

-   vollständige empfangene MeshCore-Pakete
-   RSSI
-   SNR
-   Empfangszeit
-   CRC-Status
-   weitere technische Empfangsmetadaten
-   Identität des empfangenden Nodes

Die Daten werden per WLAN/TCP an `receiver.py` übertragen.

``` text
LoRa
 ↓
Heltec V4 Repeater
MeshCore Flow + PacketTap
 ↓ TCP/WLAN
receiver.py
```

PacketTap ist eine zusätzliche Beobachtungsfunktion und soll die
eigentliche MeshCore-Funktion möglichst wenig beeinflussen.

### 3.2 WiFi-fähiger MeshCore Node

Zusätzlich soll ein **WiFi-fähiger MeshCore Node** als Datenquelle
dienen.

Dieser Node muss **kein PacketTap-Repeater** sein. Er stellt die
empfangenen MeshCore-Pakete über die vorhandene
MeshCore-/Companion-Schnittstelle bereit.

Auf der Python-Seite übernimmt beispielsweise `mc_rx_analyzer.py` die
Erfassung und Aufbereitung.

``` text
LoRa
 ↓
WiFi MeshCore Node
 ↓ TCP / Companion
mc_rx_analyzer.py
```

------------------------------------------------------------------------

## 4. Gemeinsames Ziel beider Datenquellen

Unabhängig von der Herkunft sollen alle empfangenen Pakete auf
**dieselbe Datenstruktur in QuestDB** abgebildet werden.

Dadurch können dieselben Grafana-Dashboards sowohl Daten von

-   Heltec V4 + PacketTap

als auch von

-   WiFi MeshCore Node + Companion

anzeigen.

PacketTap liefert zusätzliche technische Empfangsdaten (z. B. RSSI, SNR
oder CRC). Wenn eine Datenquelle diese Informationen nicht bereitstellt,
bleiben die entsprechenden Datenbankfelder leer.

------------------------------------------------------------------------

## 5. Komponenten

### Firmware

-   Empfang der MeshCore-Funkpakete
-   PacketTap-Erweiterung (nur Heltec)
-   Weiterleitung der Daten

### Python

-   Empfang der Daten
-   Dekodierung
-   Logging
-   Import nach QuestDB

### QuestDB

Zeitreihen-Datenbank für alle empfangenen MeshCore-Informationen.

### Grafana

Visualisierung und Analyse der in QuestDB gespeicherten Daten.

------------------------------------------------------------------------

## 6. Langfristige Ziele

-   einheitliche Datenbasis für alle Erfassungswege
-   gemeinsame Grafana-Dashboards
-   Routing-Analysen
-   Netzwerktopologie
-   Replay aufgezeichneter Daten
-   Performance- und Reichweitenanalysen
-   wissenschaftliche Auswertungen des MeshCore-Netzes
