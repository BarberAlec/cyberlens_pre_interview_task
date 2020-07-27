/** @mainpage Cyberlens Coding Challenge
*
* @author Alec Barber
*
* @section intro Project Description
* This project was completed as a pre-interview task for <A href="https://www.cyberlens.eu/">cyberlens</A>. 
* It simulates the processing of packets via multiple consumer threads. Each consumer thread accepts a packet in the form of a vector of strings 
* from the producer. Each consumer continually check for meta commands from its producer. These commands are either END or REPORT. The consumer can
* perform these actions before completing all tasks on the job queue.
* 
* 
* @section Shortcomings Shortcomings
* Their are a number of items which I feel could have had a better solutions, many with TODO comments within the code.
* 
* 
* 
* 
* @section requirements requirements
* pthread lib.
*
*/