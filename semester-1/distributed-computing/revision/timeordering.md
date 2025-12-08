### Time Ordering

## Why Synchronization

- Synchronization needed for correctness, and fairness.
- Correctness ensures that operations and events occur in proper logical order, which would prevent inconsistencies that could lead to incorrect system behaviour.
- Fairness ensures that processes or users don't experience disadvantages due to timing and Synchronization issues.
- The bus example: if your watch is 15 minutes early, you get to the bus stop 15 minutes earlier, and wait longer than necessary. This is a fairness violation.
- If your watch is 15 minutes late, you miss the bus. This is a correctness violation, as the system failed to achieve its intended outcome.

## Sync b/w Systems

- Systems have physical clocks.
- These drift over time, and different clocks drift at different rates.
- There is no absolute physical clock, that can serve as a reference for every clock in the world.
- Therefore, there will always be some misalignment between clocks in different systems.

## Example of Synchronization Problem

- Consider an online airline reservations system. Two servers, A and B.
- Customer books last ticket on airline ABC 1234.
- Server A processes the booking at 10h:15m:45.90s, and logs and timestamps the booking.
- Server A sends message to Server B saying "flight full".
- Server B receives the "flight full" message at 10h:10m:55.20s, and logs "flight ABC 1234 is full".
- When another server, say Server C, queries A and B's logs, it sees that B says flight is full at 10:10...., but A says the booking that made the flight full happened at 10:15...
- This is a discrepancy, and might cause further incorrect, and unpredictable behaviour in the system, by C.
