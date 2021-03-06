# Mitvane
Mitvane is Signature-based IDS (Intrusion Detection System) / IPS (Intrusion Prevension System) for ETSI-G5 based V2X communication.


## Prerequisites
- CMake 3.1 or higher
- Vanetza 0.8 (https://www.vanetza.org)
- yaml-cpp 0.7.0

## Getting started
1. make build directory and run CMake
```bash
mkdir build && cd build
cmake .. && make
```

2. run mitvane
```bash
sudo ./bin/mitvane -s <src_interface> -d <dst_interface>
```

## Usage
```
Allowed options:
  --help                                Print out available options.
  -l [ --link-layer ] arg (=ethernet)   Link layer type
  -s [ --interface-src ] arg (=lo)      Network interface for source.
  -d [ --interface-dest ] arg (=lo)     Network interface for destination.
  --rule-file arg (=rules.yaml)         Path to a rule file.
  --print-rx-all                        Print all received messages
  --print-rx-cam                        Print received CAMs
  --print-rx-denm                       Print received DENMs
  --print-rx-spat                       Print received SPATEMs
  --print-rx-map                        Print received MAPEMs
  -a [ --app-parsers ] arg (=ca den spat map)
                                        Run applications [ca,den,spat,map]
  -p [ --positioning ] arg (=static)    Select positioning provider
  --latitude arg (=48.766861599999999)  Latitude of static position
  --longitude arg (=11.432067999999999) Longitude of static position
  --pos_confidence arg (=5)             95% circular confidence of static 
                                        position
```

## Architecture and deployment
Mitvane is assumed to be deployed at OBU. Typically, Mitvane is connected with two network interfaces of the OBU. The source network interface is the one that captures packets on the IEEE 802.11p channel, while the destination interface is the one connected with ITS applications. The ITS applications transform data included in V2X messages and publish to ROS topics shared with the autonomous vehicle's core process. A simple application is available at https://github.com/camsenec/its_apps.

<img src=https://mitvane.s3.eu-north-1.amazonaws.com/mitvane.png>

## Detection rule file
Detection rules can be written by YAML. Here is an example to drop packet coming from outside of a circle with a radius 200m centerd on the ego-vehicle.
```yaml
  - action: "drop"
    protocol: "GeoNetworking"
    meta: 
      msg: "GBC from a source far from the ego-vehicle is detected"
      sid: 5 
      rev: 1
    allowed_so_range:
      shape: "circle"
      distance_a: 100
      distance_b: 100
```
An example detection file is available at https://github.com/camsenec/mitvane/tree/master/example_rule.

### action
The `action` determines what happens when the rule matches. The supported actions are 
  - `alert`: generates an alert when a packet matches one of the patterns defined by rules
  - `drop`: drops the packet while generating an alert

### protocol
The `protocol` tells the IDS/IPS which protocol it concerns. The supported protocol are

  - `GeoNetworking`: Network layer, for GeoNetworking header \[ETSI EN 302 636-4-1\]
  - `BTP`: transport layer, for BTP header \[ETSI EN 302 636-5-1\]
  - `CA`: application layer, for CA (Cooperative Awareness) message payload \[ETSI EN 302 637-2\]
  - `DEN`: application layer, for DEN (Decentralized Environement Notification) message payload  \[ETSI EN 302 637-3\]
  - `RLT`: application layer, for RLT (Road and Lane Topology) message payload \[ETSI TS 103 097\]
  - `TLM`: application layer, for TLM (Traffic Light Maneuver) message payload \[ETSI TS 103 097\]
  - `Facility`: application layer, for All V2X message payload

### meta
- `msg`: a message generated by an alert or drop of a packet when a packet matches one of the patterns defined by rules
- `sid`: unique ID of a rule
- `rev`: the version of a rule

### detection patterns (Updating)
Detection patterns are represented by a key-value set, where the key is one of the keywords below.
#### GeoNetworking (`protocol`: `GeoNetworking`)
| keyword | description |
|:---------:|-------------|
| `ht` | The `ht` keyword is used to check for a specific type of forwarding schemes (Header Type) of GeoNetworking. The supported types of forwarding schemes are `GBC`, `TSB` and `SHB`. |
| `mhl` | The `mhl` keyword is used to check the maximum hop limit in TSB and GBC. If the MHL of a received packet is larger than the value specified to `mhl` in the detection rule file, the| corresponding action is performed under the condition that the other patterns also matches.|
|`allowed_so_range`| The `allowed_so_range` keyword is used to check the source position vector. The keywords which consists of `allowed_so_range` are `shape`, `distance_a` and `distance_b`. With the values of these field, a range can be defined. If the source position vector does not exists in the defined range, the corresponding action is performed under the condition that the other patterns also matches.|

## Logging
format: `Timestamp [sid:rev] msg action {protocol}`

### sample
```
2022-05-09 19:14:48.958789 [2:1] GBC with large max hop limit is detected. drop {GeoNetworking}
2022-05-09 19:14:48.958960 [3:1] GBC from a source far from the ego-vehicle is detected drop {GeoNetworking}
2022-05-09 19:14:48.959029 [5:1] A packet from not trusted ITS-S is detected. (Area based) drop {GeoNetworking}
2022-05-09 19:14:48.958789 [2:1] GBC with large max hop limit is detected. drop {GeoNetworking}
2022-05-09 19:14:48.958960 [3:1] GBC from a source far from the ego-vehicle is detected drop {GeoNetworking}
2022-05-09 19:14:48.959029 [5:1] A packet from not trusted ITS-S is detected. (Area based) drop {GeoNetworking}
2022-05-09 19:14:48.959029 [5:1] A packet from not trusted ITS-S is detected. (Area based) drop {GeoNetworking}
2022-05-09 19:14:48.959029 [5:1] A packet from not trusted ITS-S is detected. (Area based) drop {GeoNetworking}
2022-05-09 19:14:48.958789 [2:1] GBC with large max hop limit is detected. drop {GeoNetworking}
```



