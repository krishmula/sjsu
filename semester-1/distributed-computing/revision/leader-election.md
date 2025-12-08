### Why leader election?

- solve coordinatio and consistency probs in distrib. systems, where multiple servers need to work together
- leader prevents scenarios which would lead to data inconsistencies.
- leaders need to be elected.
- what if:
  - existing leader fails?
  - servers disagree on who the leader is?
  - there are two leaders at the same time (split-brain)?

## The leader election problem

- in a group of processes, elect a leader
- let everyone know about the elected leader
- detect leader failure through some failure detection mechanism, and elect a new leader amongst the non-faulty processes
- all processes must agree on the same leader

## Systems model

- distributed system with n processes
- processes can fail by crashing (stop functioning)
- each process has a unique identifier (ID)
- msgs are eventually delivered (no msg loss)
- failures may occur during the election process

## The Election

- any process may call for an election
- a process can call atmost one election at a time
- multiple processes can call for elections concurrently
- shouldn't matter which process calls for the election
- at the end of the election, all non-faulty processes must agree on the same leader
- so, diff processes can't initiate elections that result in diff leaders
- Process A initiates an election, and elects Process X as leader, and Process B also simultaneously initiates an election, and elects Process Y as leader. This is not allowed.

## Guarantees

- election algo must always guarantee:
  - safety: amongst non-faulty processes, one leader with the best arbitrarily determined attribute is elected
  - liveness: for all runs of the election algo, a leader is elected who is not null, i.e. some process is elected as leader
- at the end of the algo, non-faulty process with best election attr value is elected.
- some common attr values: highest id, highest ip addr, fastest cpu, most disk space, most num of files, etc.

## Ring Election: Classical Algo

- N processes arranged in a logical ring
- each process pn can communicate with its clockwise neighbor p(n+1) mod N
- clockwise msg comm direction
- when a process detects that the leader has failed (using failure detection mech), it initiates an election.
- starts election by sending an ELECTION msg to its clockwise neighbor
- msg contains it's own ID, and attr value
- msg can be though of as a travelling ballot, that keeps accumulating info, as it moves through the ring.
- when a process receives an ELECTION msg,
  1. if incoming attr value is greater than it's own: forward it unchanged.
  2. if incoming attr value is less than it's own, and you havnen't forwarded an election msg yet: replace the attr value with your own, and forward it.
  3. if msg comes back to you: i.e. id matches your id, you are the leader, and you have the highest attr.
  4. Leader sends an ELECTED msg around teh ring, announcing itself as leader.
- There also exists a flag for each process which is set after it has participated in an election, to prevent it from participating again in the same election.

## Best and Worst Case

- Best case: the process with best attr is the initiator. N msgs are sent to determine the highest attr process, and then another N messages are sent to all the processes with the ELECTED msg.
- Worst case: the initiator process is the successor of the process with the best attr. In this case, 2N - 1 msgs are sent to determine the highest attr process, and then another N msgs are sent to all processes with the ELECTED msg. So total of 3N - 1 msgs.

## Multiple Initiators

- multiple processes may initiate elections concurrently
- each election msg carries the initiator's ID
- hierarchy based on initiator ID, to kill off weaker election runs
- every ELECTION msg carries the ID of the initiator process that started that election run
- each process remembers initiator_ID of the last election msg it forwarded.
- when process receives an incoming ELECTION msg:
  1. if incoming initiator_ID is greater than the stored initiator_ID: treat it as a new election run, and process it as per normal rules.
  2. if incoming initiator_ID is less than the stored initiator_ID: discard the msg.
  3. if incoming initiator_ID is equal to stored initiator_ID: process it as per normal rules, i.e. the id's are equal, so the process might be the leader, or it needs to forward the msg.

## Failure Handling

- when nodes fail, the ring may get broken, and liveness may be violated.

## Bully Algo

- each process knows the IDs of all other processes in the system
- when process detects that leader has failed, and if it has the highest ID amongst all the non-faulty processes, it elects itself as leader, and sends a COORDINATOR msg to all other processes, announcing itself as leader.
- if process detects that leader has failed, and if it does not have the highest ID, it sends an ELECTION msg to all processes with higher IDs, and waits for an OK response msg.
- if no process responds, it assumes that all higher id processes have failed, and elects itself as leader, and sends COORDINATOR msg to all other processes.
- if a process receiives an ELECTION msg from lower id proc, it responds with an OK msg, which shuts down the lower proc's election attempt, and then initiates its own election.

## Best and Worst Case - Bully Algo

-
