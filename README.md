# Minimal IRC Implementation (C)

A minimal implementation of the IRC protocol written in C.  
The goal of this project is to understand low-level network programming, protocol parsing, and state management by building a small IRC server and client from scratch.

Currently the project focuses on implementing a basic IRC server capable of handling connections and parsing incoming IRC messages.

---

## Current Features

- TCP server implementation
- Client connection handling
- IRC message parsing (command, parameters, trailing)
- Server state management
- Basic client structure

---

## Roadmap / TODO

### IRC Server

#### Core Protocol
- [ ] Implement command handling
  - [x] `NICK`
  - [x] `USER`
  - [ ] `JOIN`

#### Client Registration
- [x] Complete registration logic (`NICK` + `USER`)
- [x] Mark clients as registered
- [x] Send welcome reply

#### Channel Management
- [ ] Channel creation
- [ ] Add/remove clients from channels
- [ ] Maintain channel membership

#### Messaging
- [ ] Channel broadcasts
- [ ] Basic server responses

#### Networking
- [ ] Support multiple clients

---

### IRC Client

- [ ] Basic TCP client
- [ ] Send IRC commands
- [ ] Display server responses
- [ ] Interactive input

- [ ] Don't forget to check again for security considerations (usage of `gets()`, `strcpy()` ...)

---

## Project Goals

- Learn low-level socket programming in C
- Implement a real network protocol
- Understand server-side state management
- Explore protocol parsing and command dispatch