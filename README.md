# Client-Server for ASM
  
This project is about creating a client-server system in C to add networking features to a SNES mod. The goal is to let multiple computers communicate and share game data, which the original SNES hardware can’t do.

## What It Does  
- Establishes a TCP connection between a client and a server using socket programming in C.
- Sends and receives messages (like player input or commands) between two machines.
- Acts as a basic communication layer for future SNES multiplayer support.

## Tools Used  
- C programming language  
- Socket programming for networking  
- SNES emulation for testing and debugging

## Next Steps  
- Add multiplayer syncing  
- Track and change game data over the network  
- Improve debugging tools
