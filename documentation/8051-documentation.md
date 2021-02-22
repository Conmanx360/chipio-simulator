# CA0132 8051 Documentation:
Below I will attempt to document all of the info I have acquired on the
onboard 8051.

## Address Spaces:

## Known SFR Defintions:
Below is a table of the known SFR locations.

| Addr |   +0    |   +1    |   +2    |   +3    |   +4    |   +5    |   +6    |   +7    |
| ---- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- |
| 0x80 |  Port0  | Stk Ptr | DPL0    | DPH0    | DPL1    | DPH1    | DPTGL   | PCON    |
| 0x88 |  TCON   | TMOD    | TL0     | TL1     | TH0     | TH1     |         |         |
| 0x90 |  Port1  |         |         |         |         |         |         |         |
| 0x98 |  SCON0  | SBUF0   |         |         |         |         |         |         |
| 0xa0 |  Port2  | APB-A0  | APB-A1  | APB-A2  | APB-D0  | APB-D1  | APB-D2  | APB-D3  |
| 0xa8 |  IE-C   | APB-ST  |         |         |         |         |         |         |
| 0xb0 |  Port3  | HIC-A0  | HIC-A1  | HIC-A2  | HIC-D0  | HIC-D1  | HIC-D2  | HIC-D3  |
| 0xb8 |  IntPr  | HIC-ST  |         |         | HIC-SET |         |         |         |
| 0xc0 |  SCON1  | SBUF1   |         |         |         |         |         |         |
| 0xc8 |         |         |         |         |         |         |         |         |
| 0xd0 |  PSW    |         | PLL-0   | PLL-1   | PLL-2   |         |         |         |
| 0xd8 |         |         | Verb0   | Verb1   | Verb2   |         | VrbResp | IncVrb  |
| 0xe0 |  ACC    |         |         |         |         |         |         |         |
| 0xe8 |         |         |         |         |         |         |         |         |
| 0xf0 |  B      |         |         |         |         |         |         |         |
| 0xf8 |         |         | Bank-Sw |         |         |         |         |         |


## Known IRAM Locations:
Below is a table of the known IRAM locations.

| Addr |   +0    |   +1    |   +2    |   +3    |   +4    |   +5    |   +6    |   +7    |
| ---- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- |
| 0x00 | R00-B0  | R01-B0  | R02-B0  | R03-B0  | R04-B0  | R05-B0  | R06-B0  | R07-B0  |
| 0x08 | R00-B1  | R01-B1  | R02-B1  | R03-B1  | R04-B1  | R05-B1  | R06-B1  | R07-B1  |
| 0x10 | R00-B2  | R01-B2  | R02-B2  | R03-B2  | R04-B2  | R05-B2  | R06-B2  | R07-B2  |
| 0x18 | R00-B3  | R01-B3  | R02-B3  | R03-B3  | R04-B3  | R05-B3  | R06-B3  | R07-B3  |
| 0x20 |         |         |         |         |         |         |         |         |
| 0x28 |         |         |         |         |         |         |         |         |
| 0x30 |         |         |         |         |         |         |         |         |
| 0x38 |         |         |         |         |         |         |         |         |
| 0x40 |         |         |         |         |         |         |         |         |
| 0x48 |         |         |         |         |         |         |         |         |
| 0x50 |         |         |         |         |         |         |         |         |
| 0x58 |         |         |         |         |         |         |         |         |
| 0x60 |         |         |         |         |         |         |         |         |
| 0x68 |         |         |         |         | VrbNode | Vrb1    | Vrb0    | VrbVal  |
| 0x70 |         |         |         |         |         | Flag3   | Flag2   | Flag1   |
| 0x78 | Flag0   | VrbCnt  |         |         |         |         |         |         |

Addresses 0x7f and above are used for the stack.

## Known XRAM locations:


## Known Interrupt Vectors:


