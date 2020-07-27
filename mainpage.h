/** @mainpage Cyberlens Coding Challenge
*
* @section intro Project Description
* This project was completed as a pre-interview task for <A href="https://www.cyberlens.eu/">cyberlens</A>. 
* It simulates the processing of packets via multiple consumer threads. 
* 
* The program is initiated via the instantiation of a Producer object. The Producer object, constructs three 
* Consumer objects and begins their operation in a new thread (begin_thread()). Each consumer thread  continually searches both 
* the job queue and command queue for new tasks and commands respectively.
*
* There are two forms of commands that the Producer can send in the command queue. The first is a REPORT command which asks
* the Consumer to generate and print(to file) a report of all packets thus far processed. The second command is an END command
* which asks the consumer to cease searching for new jobs/tasks. The END command is always followed by a join call from the 
* Producer thread.
*
* With regards to packet processing, the consumer accepts packets in the form of a vector of strings (typedefined as csv_row).
* Once received, the consumer creates a new unique_packet object and performs a comparison check against all packets previously received 
* to check for a duplicate. If a duplicate is found, the count and index of the original packet 'unique_packet' is updated. If no duplicate is found,
* a new entry is made. One element that can be certainly improved upon is how the unique_packets are stored. Currently a vector is used. This is not ideal as
* with increasing numbers of unique_packets, the problem of checking to see if the next packet is unique becomes harder. Depending on memory restrictions,
* a solution could be the creation of a smart hashing function and associated hash map.
* 
* Both commands and jobs are passed from the Producer to the Consumer via a SharedQueue object. SharedQueue is a wrapper for a stanard queue and aims 
* to make access to the queue safe. This is achieved for a mutex which is assigned to it via the Producer and Consumer.
* 
* 
* 
* 
* @section requirements requirements
* pthread lib.
*   
* @author Alec Barber
*/