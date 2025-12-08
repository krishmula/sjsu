# Blocking v/s Non-Blocking

- Blocking: Process/thread suspended until message transmission is finished. Message transmission finish depends on protocol.

# Async vs Sync Comm's

- Sync: transmission finishes, process continues.
- Async: Proces continues as soon as message has been sent.
- Expensive to re-establish connection multiple times. So, most of the times, for sync comm, connection is kept alive.
- What is ack? For async, how does it know that message was copied to local buffer.

# Stateful v/s Stateless

-

# Handling of band data

- P2 is receiving messages. Depending on scheduling of P2, it handles messages in differnet orders. But, all messages come to P2 in the smae port.
- This is the normal band.
- Out of band data and comm. channel's are separate transmission ports.

# Applications and Comm. protocols

- Middlewares lie on top of the OS, and help in application-independent comm. b/w systems.

## Layered Comm. Protocols

- Middleware protocol should be sitting on top of our TNDP layers, just before the Application Layer, in Distributed Systems.
- This is because rarely is just TCP/IP enough of a protocol to handle all our comms.
-

# Replication

- Active and Passive Replication.

# Communication

## Message Passing -Send/receive

-

## RPC

- Not necessary to expect a return message. Usually doesn't some.

# Processing multiple requests

- Main thread accepts the requests, then sends it to a dispatcher thread, which in turn sends the requests to wherever they need to go.
- Iterative v/s Concurrent processing.

# Replicated Processses

-

# Message Brokers

- Some Message brokers services have multiple queuing layers, to maintain reliability. Some message queues don't have this layer.
