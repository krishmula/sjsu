### Scheduling

## FIFO

- maintain tasks in order of arrival
- when processor free, dequeue head, and schedule it

- avg completion time high

## STF

- maintain queue in incr order of run time
- when processor free, dequeue head, and schedule it
- avg comp time is shortest of all
- STF spec. case of **priority scheduling**

## Round Robin

- use quantum
- run portion of task during quantum
- pre-empt process by saving state of process, and resuming later
- after pre-empting, add to end of queue

## comparision

- RR preferable for interactive apps, if user needs quick responses
- FIFO preferable for batch apps

## Hadoop Scheduling

- multi-tenant - multiple customers, multiple jobs
- YARN has 2 schedulers:
  1. Hadoop Capacity Scheduler
  2. Hadoop Fair Scheduler

## Hadoop Capacity Scheduler

- has multiple queues
- each queue contains multiple jobs
- each queue guaranteed some portion of cluster capacity
- FIFO used for jobs in same queue
- admins can config queues with limits
- soft limit: how much % of cluster is queue guaranteed to occupy
- hard limit: max % of cluster to be given to queue
- elasticity: queue allowed to occupy more of cluster if resources free
- but if other queues below capacity limits, need to free resources for them
- pre-emption NO

- hierarchial queues - child and sub child queues
-

## Hadoop Fair Scheduler

- all jobs get equal share of resources
- when only one job present, occupies entire cluster
- split equally, as other jobs arrive
- divides cluster into pools
- resources div equally among pools
- within each pool, can use: Fair share sched., FIFO, etc.
- some pools have min shares %
- when min share not met, take resources away from other pools (pre-empt/kill jobs in that pool)
- if killing jobs, kill recently started jobs

- can set limits on: no of concurrent jobs per user, pool ,and no of concurrent tasks per pool

- HCS/HFS dont use SFT as running time of task is difficult to predict
- Sol is to estimate length:
  1. Within job: calc running time proportional to input size
  2. Across tasks: avg of other tasks in that given job (weigthed by i/p size)

## Dominant Resource Fair Scheduler

-
