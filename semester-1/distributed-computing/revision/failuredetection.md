### Failure Detection

- heart-beating, and ping-ack.
- topologies: centralized, ring, all-to-all.
- sync v/s async distrib system.
- sync: msg recv within bounded time. lower bound < time < upper bound.
- async: no bounds. just timeouts.

## Drift of a clock

- rate at which computer's clock deviates from perfect reference clock over time.
- physical clocks use crystal oscillators that tick at diff rates due to temp, and manufacturing variations.
- in sync dist systems, clock drift has known bound, as:
  - each proc has local clock whose drift rate from real time has a known, guaranteed upper bound.
  - msg transmission delays have upper and lower bounds.
  - proc exec steps take bounded time.
- in async dist. systems, clock drift is arbitrary, and unbounded:
  - no bounds on clock drift rates
  - msg transmission delays are unpredictable
  - proc exec speeds have no guarantee.

- The internet is an async dist system.

## Failure Model

- in sync systems, crash-stop failures can be detected using ping-ack, heartbeat, etc- there can be a timeout after which we know for sure, that proc has failed.

## Network Partition

- caused by hardware failures. (router, link, switch failing).
- network congestion (excess traffic)
- software bugs (errors in network protocols, configs)
- physical damage (cables cut, damaged)

- split brain prob:
  1. nodes in each partition believe other nodes have crashed.
  2. each partition continues operating independently, and both paritions might update same data, leading to consistency issues.
  3. when connectivity restored, sys must reconcile.

- some nodes unreachable to clients in other partition.

## Ping-Ack

- pi queries pj every T time units.
- if pj does fail, pi sends it a ping, and will time out after another T time units. So, detection time is 2T time units.

## Heartbeat protocol

- pj maintains a sequence num.
- pj sends pi heartbeat with incremented seq no. after every T'(=T) time units.
- if pi hasn't received new hearbeath from pj for the past T time units, pj is declared as failed.
- seq no. is a counter, that helps recv proc track whether new heartbeat msgs are arriving.
- so, pj sends pi heartbeat, alongside the incremented seq no., and pi checks if the seq no. has been increased. if not, declare proc as failed.

## Failure detector properties

- completeness: every proc failure is eventually detected.
- accuracy: every detected failure corresponds to a crashed proc. (no mistakes).
- tolerate some accuracy, but require 100% completeness.
- in sync systems, both protocols can satisfy both accuracy and completeness, because the upper and lower bounds are known.
- in async systems, accuracy isn't satisfied, as timeout is only arbitrary, not specified.

- in distrib systems, we want all nodes to know failure of any node.
- so, we might need a dissemination protocol, to multicast failure info to all nodes.
- reliable multicast is a dissemination protocol.

## Centralized Heart Beating

- all nodes send heartbeats to single node.
- single point of failure.
- since all processes need to know failure, the single point needs a dissemination protocol.

## Ring Heart Beating

- each proc sends heartbeats to its immediate neighbour/successor.
- each proc only monitors one proc
- when failure detected, propgate thru the ring. so, still need dissemination protocol.
- still single point of failure. one link breaks, game over.
- slow failure dissemination.

## All-to-All heart-beating

- each proc sends heartbeats to all other procs.
- each proc monitors all other procs.
- each proc maintains seq no for outgoing heartbeats to all other procs.
- when proc stops receiving heartbeats, it directly detects that failure.

- downsides are:
  1. high network traffic, all procs sending heartbeats to all other procs.
  2. total msgs: sends Nx(N-1) heartbeat msgs every T time units.
  3. can get flooded with heartbeat msgs.

## Efficiency of falure detector

- Measuring _speed_: time b/w process crash and detection. determines the speed of the failure detector
- Measuring _accuracy_: depends on the app.

## Accuracy metrics

- tmr: mistake recurrence time. time b/w 2 consescutive mistakes.
- tm: mistake duration time. length of time for which the correct process is marked as failed.
- num of false failure detections per time unit (false positives)
- num of not detected failures.

## Other failure types

- Arbitrary Failures (Byzantine)
  1. Arbitrary process failure: arbitrarily omits processing steps, or takes unintended steps.
  2. arbitrary channel failures: msg corrupted, out of order, duplicated, delayed, non existent msgs get delivered.

## Timing failures

- applicable in sync systems, as we know what time to check against.
- not applicable in async systems, as we can't say as no timing is guaranteed.
