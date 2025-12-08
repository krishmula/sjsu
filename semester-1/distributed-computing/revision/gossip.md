### Gossip

- MC might happen on TCP/UDP.
- so, packets might drop, corrupt
- nodes might crash

- simplest MC implementation is centralized.
- single point of failure

## Tree Based

- ex: IPmulticast, RMTP, TMTP
- tree setup and maintenance is a con.
- build spanning tree among processes of the MC group.
- use it to disseminate MCs.
- use ACK or NAK to repair MC not received
- SRM: uses NAK, add random delays to prevent NAK storm
- RMTP: use ACK, but ACKs only sent to designated receivers, which then retransmit missing MCs
- O(N) ACK/NAK overhead

## Another approach

- periodically, transmit to random targets.
- when a node receives it, it also transmits to random targets.
- This is on UDP, so no ACK/NAK
- b random targets per round
- called push gossip

## Pull Gossip

- periodically poll few random processes for new MC msgs you havent' received yet.

## Push vs. Pull

- Hybrid: mix of push and pull
- Push: lightweight in large groups, spread to MC quick, highly fault tolerant
- more efficient than push after N/2 processes are infected.

- Hybrid: start with push, and then transition to pull. FPTP
- both take O(logN) to reach N/2 nodes, limited by tree structure.
- for pull, as more nodes get infected, redundant contacts incr.
- as only uninfected nodes seek info, when more nodes are infected, it is quicker to spread.

## Topology aware gossip

- account for hierarchial structure of network infra
- biases target selection based on network locality
- core routers face O(logN) load, as random gossip randomly targets nodes uniformly across the system.
- as every cross submet msg must traverse the router, and on avg, half of all gossip msgs must do this

- in TAG, only one node per subnet gossips outside the subnet each round
- So, router load -> O(1), dissemination time - O(logN)

## NNTP Inter-Server Protocol

- each client uploads and downloads news posts from a news server.
- server retains news posts for a while, lazily transmits, and then deletes them after a while

## Analysis
