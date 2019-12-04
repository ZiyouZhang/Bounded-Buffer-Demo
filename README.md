# Bounded-Buffer-Demo
A C++ implementation demonstrating the bounded-buffer problem (also known as producer-consumer problem) using semaphore.

## Background
The problem describes several processes, the producers and the consumers, who share a common, fixed-size buffer used as a queue. The producer's job is to generate data, put it into the buffer, and start again. At the same time, the consumer is consuming the data (i.e., removing it from the buffer), one piece at a time. The problem is to make sure that the producer won't try to add data into the buffer if it's full and that the consumer won't try to remove data from an empty buffer. For more info check the wikipedia page here: [Producer–consumer problem](https://en.wikipedia.org/wiki/Producer%E2%80%93consumer_problem).

## Getting Started

Clone this repository and compile the code using the makefile supplied. Run the command like following (replace arguments with numbers):

```
./main <QUEUE_SIZE> <NUMBER_OF_ITEMS_PER_PRODUCER> <NUMBER_OF_PRODUCERS> <NUMBER_OF_CONSUMERS>
```

## Author

* **Ziyou Zhang** - *Algo design and coding stuff* - [LinkedIn](https://www.linkedin.com/in/ziyou-zhang/)