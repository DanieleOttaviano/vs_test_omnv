| <!-- --> | <!-- -->    |
| --- | --- |
| Channels | Region0, Region1, Region2 |
| Absolute Addresses | 0x80000000 (Region0) |
| | 0x80000004 (Region1) |
| | 0x80000008 (Region2) |
| Width | 32 bits |
| Type | mixed |
| Reset Value | 0x00 |

 ## Register Bit-Field summary

| Field         | Bit(s)    | Type  | Reset Value | Description |
| --            | --        | --    | --    | -- |
| Res0          | 31:18   | ro    | 0x0   | |
| State         | 17:16   | ro    | 0x0   | 11 : Region is full off (Ready for bitstream load)<BR>10: Region is in shutdown but not yet in reset mode<BR>1 : Region is going in shutdown mode<BR>0: Region is up and running |
| Res0          | 15:9    | ro    | 0x0   | |
| Shutdown      | 8         | rw    | 0x0   | 1 : Command region being shutdown mode<BR>0: Command region not being in shutdown |
| Res0          | 7:0     | ro    | 0x0   | |




