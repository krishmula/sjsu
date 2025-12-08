1(a) **TODO**

- There might be issues with availability with an increasing workload, which would mean that there might be a series of failures across the system.
- The system might be struggling to scale as response times start increasing, and latency becomes noticeable.
- Another metric of measuring if a distributed system is struggling is that resource utilization shoots up due to inefficient resource sharing techniques. [source from https://distributedsystemsmanagement.com/article/Key_Metrics_for_Measuring_the_Performance_of_Distributed_Systems.html]

1(b)

- Large scale e-commerce platform means a lot of users, and a lot of data that needs to be handled. So, to reduce communication latency, we might want to start by making the communication async.
- Then, we implement a distributed system, by scaling horizontally. This adds more bandwidth to handle scale, and also adds fault tolerance.
- Then, we replicate our sever, and state instances (DB), so that we can have availability and faster read speeds.

2(b) Imagine you are designing a distributed database for a real-time collaborative editing application (e.g., Google Docs). What strategies would you use to manage consistency while still benefiting from replication and caching?

2(a)

- Replication improves performance by providing high availability. Even if a data store goes down, a replica can be operational in a negligible amount of time.
- Latency can be reduced as replicas can be strategically placed closer to the locations of the consumers.
- Caching can help in quicker data retrieval as it stores frequently accessed data, in close proximity to the consumers.
  2(b) **TODO**
-

3(b) For each attribute, describe a real-world challenge that arises when implementing it and propose two techniques to address the challenge, providing examples where applicable.

3(a)

- Interoperability is important because the end user shouldn’t be able to see under the hood. They don’t really need to know what server they’re connected to. The expectation is that no matter what, the experience remains consistent. Interoperability helps us achieve that.
- When writes happen, users expect it to be propagated all throughout the system, regardless of the number of replicas or nodes in the system. On the next read, regardless of what node/replica the software layer chooses to read from, the write should be updated there. Consistency helps us with that.
- Regardless of the number of users accessing the site/product, or hardware issues with infrastructure, the expectation is that services never go down, and cause an inconvenience. Implementing replicas helps us with that via the principle of consistency.

3(b) **TODO**

4(a) In distributed system design, what is the advantage of separating concerns between control logic and execution mechanisms?
4(b) How does this separation contribute to flexibility and maintainability in large- scale systems?
4(c) Discuss a distributed system that benefits from this separation and explain how it enhances the system's scalability or fault tolerance.

Ans.
(a)
(b)
(c)

5.

- Failure transparency is the process of hiding the failures and subsequent recovery of resources. This means that the resource completes it's operation regardless of software or hardware failures.
- It is difficult to achieve because there is a dillema about whether to inform the users of the failures, and let them have the choice on how to proceed next, or keep masking the failures, until the process is successful.
- An example is when there are network or server faults during e-mail transmission, and the faults are covered up while attempting to re-transmit the e-mail, even if it takes multiple days to do so.

6(a) **TODO**

- Making a distributed system too transparent, leads to operations being obscured, and thus, increasing latency, which might slow down the system instead of gracefully exiting.
  6(b)
- Reducing transparency in some situations can help users make the more rational choice in case of network delays or failures, which can improve system reliability.
- For example, when a print operation fails, instead of opaquely re-routing the operation to some printer in the building, a user could be given the choice to re-route it to their nearest printer.

7(a) Explain the differences between caching and data replication in distributed systems, focusing on their roles in improving performance and consistency.
7(b) How do distributed databases use both caching and data replication to ensure fast data access and high availability?
7(c) What are the potential challenges of maintaining data consistency in systems that rely heavily on replication, and what strategies can be employed to address these challenges?

Ans.
7(a)

- Cache stores only frequently accessed data in a temporary fashion, in close proximity to the consumer, so that there is lesser latency in accessing the data.
- Data replication is the process of creating and maintaining copies of the same data in different locations to ensure data availability. [from https://www.ibm.com/think/topics/data-replication]
- Cache and data replication both increase availability of data stores, but differ in that if a data store goes down, it has replica's to take it's place. While cache is just a store for frequently accessed data, not a full blown data replica.

7(b)

- Caching can help access data faster as it stores frequently accessed data in a location in close proximity to the consumer, which covers availability.
- Data Replication can give us high availability, as we would have multiple replicas of data stores in case one goes down.

7(c) -> **TODO**

- When there are a lot of replicas present, making all the data stores consistent can be a chore.

8(a) How do performance, reliability, and scalability each impact the design and operation of a distributed system? Discuss the potential conflicts that can arise when trying to optimize these attributes simultaneously.
8(b) In the design of a high-traffic online platform, what considerations would guide decisions on prioritizing one attribute over another? How would you approach balancing the need for fast response times, system uptime, and the ability to handle increasing user demand?.

Ans.
8(a)
8(b)

9. Below is an illustration of three-tier computation:

   Compare these designs and discuss:

9(a) What are the benefits of shifting certain tasks from the application or database layers to the client-side in a distributed system?
9(b) In what scenarios can client-side processing enhance system scalability, reduce server load, or improve the overall user experience? Provide specific examples to illustrate the advantages of this approach.

Ans.
9(a)
9(b)

10(b) How does achieving various levels of transparency enhance usability and system manageability in distributed systems?
10(c) In certain situations, it may be beneficial to limit transparency. Provide an example of when reducing transparency can improve performance, security, or system reliability, and explain why it is necessary.

Ans.
10(a)

- Transparency is the attempt to make the under the hood implementation of distributed systems opaque to the consumer and programmer so that the system is perceived as a single unit instead of a complex collection of components. 1. Access transparency allows resources to be accessed regardless of location (local v/s remote) during operations. 2. Location transparency allows resources to be accessed without having to know their location. 3. Concurrency transparency enables proecesses to run parallely without having to worry about how to share resources between themselves. 4. Replication transparency allows multiple replicas to exist and be used without having to know the details about the remainder of the replicas. 5. Failure transparency hides the failures of the system's resources, so that user's processes can be carried out even after hardware and software failures. 6. Mobility transparency allows relocation of resources in the system without affecting ongoing operations. 7. Performance transparency allows system to be reconfigured to improve performance as loads scale. 8. Scaling transparency allows system to scale without any change in the structure of the system, or modification of existing algorithms.
  10(b)
  10(c)
