### Peer to Peer Systems

- key prob in p2p is placement of data objs across hosts, and accessing them later.
- p2p has been used for file sharing, web caching, info distrib, etc, across the internet.
- best used for storing large collection of immutable data.

- goal is to share data & resources on a large scale, and not have any dedicated servers for it.
- each node contributes resources (data, computer, network bandwidth, etc).
- no centralized dependencies.
- more the number of users join, more the available resources.

- availability of processes and computers in p2p systems is unpredictable.
- owner of computer might turn it off.
- so, no guarantee of individual resources can be given.
- solution is replication.

## P2P Middleware

- Pastry, Tapestry, Kademlia, CAN, Chord are a few p2p middlewares.
- these are designed to place resources on a set of computers widely distrib throughout the internet.
- provide guarantees of delivery of requests in a bounder no. of network hops.
- they place replicas of resources on available host computers in a structured manner.
- resources identified by GUID, derived from secure hash.

## Napster

- architecture included centralized indexes, but users supplied the files.
- files stored on users computers.
- all comm uses TCP
- so, reliable, and ordered comm

- clients connect to napster server.
- clients upload a list of music files they want to share.
- server maintains list of <filename, ip, portnum> of these files now.
- congrats! you are sharing files on napster

- to search, clients send server the keywords of the music file they want.
- uses ternary tree algo to search
- server searches it's index for the records pertaining to the keywords
- server returns a list of hosts <ip, portnum>
- client pings each host, to find the transfer rates.
- client fetches file from the best host.

- Problems:
  1. Centralized point of failure.
  2. congestion at centralized servers
  3. no security: plaintxt msgs and pswds
  4. copyright infringement

## DHT

- hash table in distrib setting. (objects with keys. objects -> files)
- napster, chord, gnutella, are all dhts.
- concerns: load balancing, fault tolerance, lookup & insert efficiency, locality

- use consistent hashing to map both data items and nodes(peers) onto the same identifier space.
- each node and data is assigned unique identifier (gen by hash func).
- identifiers are then truncated to m bits, creating identifiers in the range 0 - 2^m -1
- then mapped on to a logical ring structure.

- data stored at the first peer whose identifier is equal to or greater thatn the data's hashed key.
- ex: if file hashes to key K42, it's stored at the first node with an identifier >= 42 on the ring.
- with K keys & N peers, each peer stores approx. O(K/N) keys on avg.

- to locate data, nodes maintain routing tables (finger tables) containing pointers to other nodes at specific positions around the ring.
- when searching for key, node forwards the query to largest successor/finger entry that doesn't surpass the key's hash value.
- achieves O(logN) search time.

## Chord

- maps both peers and files onto circular id space.
- SHA-1 converts ip and port no. (peer info), and filenames (file info) into 160 bit strings, then truncated to m bits.
- identifiers b/w 0 and 2^m -1
- ith finger entry points to first node at or clockwise of position (n + 2^i) mod 2^m.

- handles failures thru replication and stabilization.
- each node maintains r successor entries as backup, so, if immediate successor fails, next available one handles it.
- stabilization protocol runs periodically at every peer, to check and update pointers, ensuring eventual consistency despite **churn**

- introducer node directs new join nodes to appropriate posi on the ring.
- new node initializes its successors and finger tables by querying neighbours, and must copy the relevant files from its successors.

## Search under Peer Failures

- maintain r successor entries in case of failure.
- maintain replicas in r successors and predecessors.

## Churn

- a new peer affects O(logN) other finger entries in system, on avg.
- no of msgs per peer join: O(log(N) \* log(N))

- hourly peer turnover rates could be 25-100% of total no of nodes
- stabilization protocol needs more bandwidth to keep up
- might be sufficient to only replicate meta data about files
- Alternatives:
  1. indirection. store only pointers to files
  2. replicate metadata more

- strong stabilization might take O(N^2).
