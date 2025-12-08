Ans 1.
i). Accessing a virtual terminal through Telnet, or SSH is expected to be a reliable affair. Any unreliability would render a session useless.
a. When implemented via UDP, some typed characters might go missing, or appear in the wrong order, as every datagram is considered an individual message, and is transmitted according to some optimization algorithm.
b. If implemented using TCP, since a dedicated connection is established, and every segment has a sequence number, the data is sent in the exact order, and any corrupt data segment is retried until it's gotten it right.

ii). Transfering files between computers is also expected to be reliable. You want the exact same file in both systems.
a. Using UDP, chunks of the file might appear out of order, and the onus to re-arrange it falls on the receiver. Also, some chunks of the file might disappear, due to inconsistent network, and this might cause us to have corrupt files.
b. Using TCP, we know that file chunks are transmitted in the exact order, and that if the checksums don't match between the sender, and receiver, the respective file chunks are re-sent until it gets it right.

iii). **_ Determining using location using finger is usually an ad-hoc, on demand process. When finger is called, it queries the system, and returns the current state of the system. While, a utility like rwho queries the system status dump that a daemon runs every minute or so. So, it doesn't transmit live information, but information from the latest dump.
a. 1. When implemented using TCP, finger might take a little longer due to establishing handshake (as the actual querying and returning sys info is relatively trivial, and can be done in very short amount of time), but it does return the info reliably, and without any missing info. 2. rwho also transmits the info reliably, and has the same issue of taking too long to return info sourced from a sys info dump.
b. When implemented using UDP, finger can quickly return the sys info from the dump, instead of having to wait for a handshake. If the data transmission fails (due to the volatile nature of UDP), it can just _**

iv). Web browsing needs perfectly functional HTML to be rendered, for users to view web pages. Any broken, HTML, CSS or JS, can result in unviewable pages.
a. When implemented using UDP, there is potential for chunks of HTML, CSS, or JS to be lost, and thus, breaking the web view for the end users.
b. When implemented using TCP, all the HTML, CSS, and JS is perfectly received, and if there is an error even in a single data packet, the entire packet is retransmitted, ensuring that nothing breaks due to missing HTML, CSS, or JS.

v). a. When implemented using TCP, the request to another process, and the reply/acknowledgement happens reliably, as it happens over a persistent connection. And because TCP is a reliable protocol, and has an open connection, large amounts of data can be returned without hassle.
b. When implemented using UDP, sometimes the request/reply may not go/come through, so this means the processes must have some kind of retry mechanism built in to execute the RPC.

Ans 2.
Ports usually have only one receiver, because a socket usually communicates with a port using it's local internet address, and it's port number. So, by this notion, a socket owns a port.
But, for multicast communication, a group of processes/sockets are part of a multicast group, which is subscribed to a port. This means that whenever communication happens through the port, it is sent to all the members of the multicast group. The sender doesn't know about the number of processes subscribed to the port.
It is useful for a port to have multiple subscribers/receivers, to:
a. Replicate the service, as even if a few subscribers go down, the operation can still be seen through via the standing processes.
b. When data changes, the multicasting via a port can be helpful to propagate the changes to all the subscribed processes, thus bolstering the replication.
c. Multicasting event notifications. Processes can be notified of events via multicast and the publish-subscribe model. This can be made possible due to a port being able to handle multiple receivers.

Ans 3.
Messages can be dropped during IP multicast due to the nature of UDP. To ensure that there are no messages missed by the receivers:
a. When a datagram is sent, the sender should receive an acknowledgement of it being received in order to proceed. If the acknowledgement doesn't come, it means that the message has been dropeed. And that the sender should retransmit the message. This should help curb the problem of dropped messages.
b. But, this can make for a very confusing set of acknowledgements between multiple senders. So, we can have a unique identifier for each sender, which is also transmitted appended to the data in the datagram. So, when we don't hear back an acknowledgement, we know what sender has to re-transmit.
c. This should work well with the assumption that only a small proportion of messages will be dropped, thus, not causing a huge traffic of acknowledgements.

Ans 4.
Yes, a message routed through DNS would exhibit the same level of indirection, i.e. None. It would still be time- and space-coupled. This is because:
i. When the sender provides the hostname as the address, rather than the id of the receiver, the DNS resolver kicks in to action, and finds the id of the receiver, and **_provides back the ID_** to the sender.
ii. It doesn't obscure the ID from the sender, it just provides the ID back to the sender. So, it is not actually an intermediary between sender and receiver, it's just an extra step.
iii. Time isn't uncoupled as well, because if the sender and receiver are independent of each other, i.e. if the receiver is down when the message is sent, the message wouldn't be passed on at all. The message passing exercise would fail.

Ans 5.
The notification mailbox service is going to be a time and space uncoupled system.
i. It is going to have multiple subscribers, who indicate their availability to have messages delivered by setting a flag to indicate when they're up and ready for delivery. When the flag is not set, it means they're either down, or not ready for delivery.
ii. To transmit messages to multiple subscribers, we can use a reliable multicast protocol, so that receivers that are up would receive their messages, instead of nobody receiving the message with atomic multicast.
iii. We are going to have a storage/repository that can store messages alongside metadata on where to be delivered, so that we can time-uncouple the senders and receivers. When we multicast a message, we are going to be waiting for an acknowledgement of the receiver receiving the message. When it isn't received, or when the flag isn't set, we append that metadata to the message, and store it in our repository. And we can ping those receivers periodically to see if they're active/if their flag has been set, and if so, we re-trany the message to those receivers.
iv. If a receiver crashes during their delivery window, we wouldn't get the acknowledgement back from them. So, we store the message alongside their id, etc. as metadata, in our message storage/repository, and we ping the receiver periodically again, and re-transmit the message when they're available.
