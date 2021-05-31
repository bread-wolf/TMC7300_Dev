# ComStack

Simple UART-Based communication tool.

## Specification:

| Byte | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
|---|---|---|---|---|---|---|---|---|---|---|
| Request frame | Slave Address | Job ID | Command | Channel | Register Address | Data [3] | Data [2] | Data [1] | Data [0] | Checksum |
| Reply Frame | | | Master Address | Job ID | Status | Data [3] | Data [2] | Data [1] | Data [0] | Checksum |

Simple Master/Slave concept, Job ID allows for asynchronous operation of the communication stack.

### Addresses :

Slave addresses allow one master to control multiple slave modules. Master address on the contrary is fixed to 0x01 and is reserved for the master.

### Frame ID :

Job ID is dictated by the master, which has the task of remembering the frame type until the request has been fulfilled.

### Command byte :

TMCL-Specific commands:
 - TMCL Read Register
 - TMCL Write Register

### Channel:

Each module can have multiple channels, these are used to communicate with multiple devices which are slave to the module. For example a microcontroller with several Trinamic chips attached, or other register based sensors. This also support commands for virtual registers.

### Data:

Each frame can have up to 4 data bytes in this basic configuration. This is mainly meant for reading and writing registers, and debugging simple application at low command rates.
If high command rates are ever needed, I will add RAM, or SPI Flash based debug, where data is first streamed to memory at high speed, and retrieved later. We could also put in some kind of streaming more with a more optimized frame.

### Checksum:

Simple 8 bit sum, the purpose is only to detect inadequate bus topology, not to have active correction behaviour. The goal is just that the user knows that something is wrong, and it has to do with connection, cable length, EMI, etc. nothing more.

## Minimal implementation :

- [ ] Support TMCL-Read and Write
- [ ] Python register editor for TMC7300
- [ ] Support Feature discovery
- [ ] Support Channel discovery

