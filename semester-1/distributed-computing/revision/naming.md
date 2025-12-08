### Naming

- Address: name of an access point, location of an entity
- Name space: contains all valid names recognized and managed by a service
- Naming Domain: name space with a single admin authority which manages names for the space.

## Naming Design Principles

- Data Structure:
  1. No Structure: uuid
  2. Complex: include location info/reference, etc. ex: GLN
- no one system can have complete view of all names

- ip + port + filepathname -> identifies a resource on a server.
- ip -> identifies network address

## Flat Naming

- unstructured
- identifiers have no structured description
- just a set of bits
- no additional info about the entity
- ex: internet address at network layer, m-bit no. in DHT suitable for dynamic systems where nodes form a system without centralized coordination. so, systems with high rates of churn

## Broadcast based name resolution

- broadcast identifier of entity being sought across the network
- when bcast msg reaches all nodes, only entity matching identifier will respond by returning its access point info
- ex: ARP. map IP to MAC addr at the data link layer.

## Structured naming

- names org into name space which can be modeled as graph.
- leaf node rep entity; nodes are also entities.

- Hard Links: multiple absolute pathnames referring to the same node
- Sym Links: leaf node storing an absolute path name

## Mounting

- a directory node in a remote server can be mounted on to a local node.

## Name space implementation

- serveral servers used to manage names
- distrib layers:
  1. Global layer: root node and some of its close nodes
  2. Admin Layer: directory nodes managed within single org
  3. Managerial Layer: nodes typically change regularly

## Name resolution

- Iterative at Client side: at resolver side. name resolver performs multiple queries themselves. so, servers return referrals not to client, but to themselves.
- Iterative at Server side: servers keep returning referrals to the next appropriate server, rather than performing the query themselves.
- Recursive: server handles resolution workload. servers query amongst themselves and return the result.

## Attribute Based Naming

- allows entities to be located based on their characteristics rather than names/posi in hierarchy
- also called directory services
- resolution uses querying mechanisms rather than direct lookup

## LDAP

- uses obj class to describe entities with <attr, value> tuples.
- organizes them into Directory information tree
- defined by a distinguished name (DN)
- client-server protocol where Directory User Agents (clients) query Directory System Agents (servers), that manage fragments of Directory Information Base (DIB)
