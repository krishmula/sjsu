## What are objects?

- objects in DS have some state.
- objs have state, and they expose some methods to interact with, and call them.
- comm with each other using RMI.
- each obj has a _remote obj reference_
- serves as a globally unique identifier, allowing it to be referenced and invoked anywhere in the DS.
- each obj's behavior is specified through _remote interfaces_ that provide a specification of the methods available for invocation.
- interfaces defined using **IDL**.

- objs must handle distrib exceptions (message loss, process failure) from netw comm.

## Communication Paradigms

- Three types of comm paradigms.

1. Interprocess Comm
2. Remote invocation
3. Indirect Comm.

- below are techniques that represent a two way relationship b/w sender and receiver.
- sender sends msgs directly to receivers, and receivers know exactly who the senders are.
- So, _direct communication_

1. **Interprocess Comm**:
2. **Remote invocation**:
3. **Request-Reply Protocol**:
4. **Remote Procedure Calls**:
5. **Remote Method Invocation **:

- Now, _indirect comm_

1. **Group Communication**:
2. **Pub-Sub**:
3. **Msg Queue**:
4. **Tuple Spaces**: processes place items of structured data in a persistent tuple space, which can be accessed by other process (read/remove). Readers and writers don't need to exist at the same time.
5. **Distributed Shared Memory**: reading or writing shared data structures. Underlying infra must ensure a copy is provided in a timely manner, and also deal with sync and consistency of data.

## Process v/s Thread

- process is the program being executed by the OS.
- threads are some things within the process.
- switching thread context cheaper than process context.
- each process has its own address space. threads share the parent processes address space.
- data sharing in processes req msg passing, etc. threads don't, as they're in same addr space.

## Comm Protocols

1. **Service Location Protocol**:

- enables newly connected computers to discover local network services when they move b/w diff netw locations.
- _service agents_ advertise availability of service on behalf of service providers. these agents also respond to discovery req from clients.
- _user agents_ perform service discovery on behalf of client apps. they send out discovery reqs.
- _directory agents_ maintain centralized repos of service ads. allow user agents to query a single location rather than broadcasting reqs across entire network.

2. **Message Passing Interface (MPI)**:

- standardized API for msg passing.
- processes comm by sending and receiving msgs.
- each process has access to MPI library buffers, which temporarily hold data in transit.
- comm happens either: point-to-point (b/w two spec processes), or collective (multiple processes: multicast, broadcast, scatter, gather, etc)
- MPI_Send, MPI_Ssend, MPI_Bsend, MPI_Rsend, MPI_Isend, MPI_Irecv.
- provides sync, async, blocking, non-blocking semantics.

## Blocking v/s Non-Blocking Comm Calls

- Persistent Comm: msgs are kept in commsystem until they're delivered to receiver.
- Transient Comm: msgs are kept in connec temp only if sender and recv are live.

## Async vs Sync Comm

- async: process continues as soon as sending msgs have been copied to local buffer. non-blocking send; recv blocking/non-blocking

- sync: sender waits until it knows msgs have been delivered to receiver. blocking send/ blocking recv. and typically use connection-oriented, keep-alive connecs.

## Comm Protocols

- TCP/IP: App Layer: SMTP, HTTP, Telnet, FTP, etc. Transport Layer: UDP, TCP, Internet Layer: IP, Link Layer: Most network hardware.
- IP: datagram is the basic data unit. Defines the internet address scheme.Transmits data between the Network Acess Layer, and the Transport Layer. Divides and assembles datagrams.

- TCP:
  - TCP: Segment. it is the original data + Transport layer header.
  - UDP: Packet. it is segment + Network layer header.
  - Link Layer: Both TCP and UDP send data as frames (packet + data link layer header.

  - SYN -> SYN, ACK(SYN) -> ACK(SYN), request, FIN -> ACK(req + FIN), answer, FIN -> ACK(FIN).

## Middleware

- general purpose, but app-specific protocols, middleware comm protocols.

## Ports

- transport layer provides a network independent message tranport service b/w pairs of network ports.
- ports are software defined destination points at a host computer.
- ports are attached to processes, enabling data transmission to a specific process at a specific destination node.
- transport address: network address + port no.
- TCP and UDP must support process-to-process comm, which is accomplished using ports.
- port no. -> 16-bit integer.

## Interprocess Communication

- the API to UDP provides a message passing abstraction.
- sending process transmits a single msg to recv process.
- independent packets containing these msgs are called **datagrams**.
- sender specifies destination using socket - an indirect reference to a particular port used by destination process at destination computer.

- API to TCP provides abstraction of a two way stream b/w pairs of processes.
- info commed consists of a stream of data items with no msg boundaries.
- producer and consumer form a pair of processes (produce data items -> consume them).
- data items sent are queued on arrival at recv host until consumer is ready to recv.

- An **overlay network** is a network that is built over another network to permit applications to route msgs to destinations not specified by an IP address.
- Overlay networks enhance TCP/OP networks by providin alternative, specialized network services.
- they're important in supporting multicast, and p2p comm.

## Sync and Async Comm

- queue associated with each msg destination.
- send proc -> msg added to remote queue, recv proc -> remove msg from local queue.
- in sync commm, send and recv proc sync at every msg. both, send and recv are blocking operations.
- when a send is issued, the sending proc/thread is blocked until corresponding receive is issued.
- when a receive is issued by a proc/thread, it blocks until msg arrives

- in async comm, send operation is non-blocking, i.e. sending proc allowed to proceed as soon as msg is copied to local buffer.
- transmission of msg proceeds in parallel with sending proc.
- receive operation can have blocking and non-blocking variants.
- in non-blocking variant, recv proc. proceeds with it's program after issuing a recv operation.
- a buffer is provided to be filled, in the background.
- it seperately receives the notif that buffer has been filled (polling or interrupt).

- today's systems, non-blocking recv is not usually common, as we have multiple threads in processor architecture now.

## Message destinations

- local port is msg destination.
- port -> exactly one receiver (except multicast ports), i.e. only one process can receive the msgs sent to a particular port.
- port -> many senders.
- but, processes may use multiple ports to recv msgs.
- see fig. 4.2 on pg 149 of textbook

## Sockets

- comm is basically transmitting msg b/w socket in one proc to socket in another proc.
- its socket must be bound to a local port and one of the internet addresses of the computer on which it runs, for a process to recv msgs.
- proc may use same socket to send & recv msgs.
- 2^16 possible port num's for use by local procs for recv msgs on any computer.
- proc can use multiple ports, but a port can't have multiple procs on it (except for IP multicast).
- each socket is associated with either TCP or UDP.

## UDP Datagram Comm

- datagram comm, no ack or retries. if failure, msg may not arrive.
- to send/recv, proc first need to create a socket bound to an internet address of the local host and a local port.
- server binds socket to server port (makes it known to clients that it can recv msgs from clients).
- client binds to any free local port.
- _receive_ method returns internet address and port of sender, alongside msg.
- allows receiver to send a reply.
- to be clear, receive is a method that is meant to receive the msg on the receiver (that's being sent by the sender).

- Some problems:
  - **Message Size**: recv proc needs to specify size which it can receive. if msg too long according to size, msg is truncated.
  - **Blocking**: sockets normally do non-blocking sends, and blocking recvs. send operation returns after it drops msg off to the underlying UDP/IP protocols, and on arrival, msg is placed in queue for dest port's socket. Msg can be collected from the queue by an outstanding/future invocation of the _receive_ method.
  - **Timeouts**: recv can sometimes wait indefinitely, as send proc might've crashed, or msg might've been lost. So, timeouts are set on sockets, after which recv is no longer blocking.

- Failures:
  1. Omission Failures: msgs may be dropped, due to checksum errors, or no buffer space being available at source/dest.
  2. Ordering: msgs sometimes delivered out of sender order.

- DNS, VOIP implemented over UDP.

- UDP doesn't suffer from overheads of guaranteed msg delivery.
  1. need to store state info at source and dest.
  2. transmission of extra msgs.
  3. latency for sender.

## TCP Stream Comm

- API to TCP provides abstraction of a stream of bytes.
- data may be written to the stream, and read from the stream.
- Some hidden characteristics:
  1. **Message sizes**:

## Remote Object References

- client invokes remote obj method, msg sent specifying what obj is to have its method invoked (msg sent to server).
- **remote obj reference** is a identifier for remote obj throughout distrib system.
- so, **ror** of obj whose method we want is passed in invocation msg.
- ror's shouldn't be deleted even after the obj associated with it is gone. and any attempt to invoke deleted obj using that ror should throw error.
- unique ror -> concat internet addr of host and port num of process that created it + time of creation + local obj no.

## Multicast Comm

- simplest MC provides no guarantees about msg delivery or ordering.
- useful for:
  1. multicast messages to a group of replicas of something.
  2. MC msgs used to locate available discovery services
  3. maintain consistency in a group of replicas.
  4. event notif. like facebook, etc.

## IP Multicast

- built on top of IP.
- allows sender to transmit single IP packet to a set of computers(multicast group).
- sender doesn't know receivers.
- specified by Class D internet addr.
- memebership of MC groups is dynamic.
- can send datagrams to MC group without being a member.
- at app prog level, MC only available via UDP.
- program can join MC group by making it's scoket join the group.
- at IP level, a computer belongs to an MC group, if one or more of its processes has sockets that belong to an MC group.
- MC routers help MC on the wider internet.
- Class D Addresses (224.0.0.0 -> 239.255.255.255) reserved for MC traffic.
- Suffer from same probs as UDP.
- some, but all members might not receive msgs. This is also called **unreliable multicast**.

## Overlay Networks

-

## MPI
