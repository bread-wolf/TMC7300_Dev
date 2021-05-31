# ComStack

Simple UART-Based communication tool.

## Specification:

| Byte | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|---|---|
| Request frame | Slave Address | Frame ID | Command | Channel | Register Address | Data [3] | Data [2] | Data [1] | Data [0] |
| Reply Frame | | | Master Address | Frame ID | Status | Data [3] | Data [2] | Data [1] | Data [0] |

Simple Master/Slave concept, Frame ID allows for asynchronous operation of the communication stack.

### Addresses :

Slave addresses allow one master to control multiple slave modules. Master address on the contrary is fixed to 0x01 and is reserved for the master.

### Frame ID :

Frame ID is dictated by the master, which has the task of remembering the frame type until the request has been fulfilled.

### Command byte :

TMCL-Specific commands:
 - TMCL Read Register
 - TMCL Write Register

### Channel:

Each module can have multiple channels, these are used to communicate with multiple devices which are slave to the module. For example a microcontroller with several Trinamic chips attached, or other register based sensors. This also support commands for virtual registers.

### Data:

Each frame can have up to 4 data bytes in this basic configuration. This is mainly meant for reading and writing registers, and debugging simple application at low command rates.
If high command rates are ever needed, I will add RAM, or SPI Flash based debug, where data is first streamed to memory at high speed, and retrieved later. We could also put in some kind of streaming more with a more optimized frame.

## Minimal implementation :

- [ ] Support TMCL-Read and Write
- [ ] Python register editor for TMC7300
- [ ] Support Feature discovery
- [ ] Support Channel discovery

