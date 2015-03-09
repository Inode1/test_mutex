/*
 * test_mutex2.cpp
 *
 *  Created on: Mar 6, 2015
 *      Author: ivan
 */

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

struct trace_queue
{
   enum { LineSize = 100 };

   trace_queue()
      :  message_in(false)
   {}

   //Items to fill
   char   items[LineSize];

   //Is there any message
   bool message_in;
};

typedef struct msg_struct_
{
    unsigned int           data_len;
} msg_struct;

typedef struct control_struct_
{
	pthread_mutex_t  mutex_var;
	pthread_cond_t   cond_var;
	unsigned int   msg_size;
    char           msg_count;
    char           msg_status;
    char           read_state;
    char           write_state;
} control_struct;


#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>
#include <cstring>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/named_condition_any.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

using namespace boost::interprocess;

int main ()
{

	try{
		std::cout << "Start" << std::endl;

    shared_memory_object shm
          (open_only                    //only create
          ,"MRSTPSharedMemory"              //name
          ,read_write                   //read-write mode
          );
/*
    std::cout << "Start" << std::endl;
    //Condition to wait when the queue is empty
    boost::interprocess::named_condition  cond_empty(open_only, "cond_empty");

    //Condition to wait when the queue is full
    boost::interprocess::named_condition_any  cond_full(open_or_create, "cond_full");*/
    std::cout << "Start" << std::endl;
    //Create a shared memory object.
/*   shared_memory_object shm
      (open_only                    //only create
      ,"MRSTPSharedMemory"              //name
      ,read_write                   //read-write mode
      );*/

   std::cout << "Start" << std::endl;
      //Map the whole shared memory in this process
      mapped_region region
         (shm                       //What to map
         ,read_write //Map it as read-write
         );

      //Get the address of the mapped region
      control_struct *pointControl = static_cast<control_struct *>(region.get_address());
      //Obtain a pointer to the shared structure

      //Print messages until the other process marks the end
      std::cout << "Start" << std::endl;
      bool end_loop = false;
      std::cout << "Out mutex" << std::endl;
      do{
    	  pthread_mutex_lock(&pointControl->mutex_var);
         std::cout << "In mutex" << std::endl;
         if(!pointControl->msg_status){
             std::cout << "Wait" << std::endl;
             pthread_cond_wait(&pointControl->cond_var,&pointControl->mutex_var);

         }
         sleep(2);

         pointControl->read_state %= pointControl->msg_count;

         size_t offset_from_begin= (pointControl->read_state * (pointControl->msg_size + sizeof(msg_struct))) + sizeof(control_struct);

         msg_struct *msg_point = (msg_struct *)((char *) pointControl + offset_from_begin);
         char *data = (char *)(&msg_point->data_len + 1);
         std::string result(data, msg_point->data_len);

         ++pointControl->read_state;

         if(std::strcmp(data ,"last message") == 0){
            end_loop = true;
            std::cout << data << std::endl;
         }
         else
         {
             std::cout << result << std::flush;
         }
         pointControl->msg_status = 0;

         pthread_mutex_unlock(&pointControl->mutex_var);
         std::cout << "Out mutex" << std::endl;
         sleep(2);
      }
      while(!end_loop);
   }
   catch(interprocess_exception &ex){
      std::cout << ex.what() << std::endl;
      return 1;
   }

   return 0;
}
