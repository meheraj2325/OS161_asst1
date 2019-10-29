#include <types.h>
#include <lib.h>
#include <synch.h>
#include <test.h>
#include <thread.h>

#include "invest_assignment.h"
#include "investor_producer.h"

static struct semaphore *item_mutex;
static struct semaphore *order_take_full;
static struct semaphore *serv_con_full[NCUSTOMER];
static struct semaphore *bank_mutex;

static int count_serve_orders[NCUSTOMER];
static int count_order_taken;

/*

// For degugging purposes

static int count_consumed[NCUSTOMER];
static int total_loan_reimberse[NPRODUCER];
static int count_served_by[NPRODUCER];
static int total_order[NCUSTOMER];
static int iterations = 0;
static int No_of_order_taken = 0;

*/



/*
 * **********************************************************************
 * FUNCTIONS EXECUTED BY ITEM CUSTOMER THREADS
 * **********************************************************************
 */


/*
 * order_item()
 *
 * Takes one argument specifying the item produces. The function
 * makes the item order available to producer threads and then blocks until the producers
 * have produced the item with that appropriate for the customers.
 *
 * The item itself contains the number of ordered items.
 */ 

void order_item(void *itm)
{
    // (void)itm; // to avoid warning
    //panic("You need to write some code!!!!");

    P(item_mutex);

    struct item *ordered = itm;
    struct item *head = req_serv_item;
    int i = 0;


    if(head==NULL)
    {
        req_serv_item = (struct item*) kmalloc(sizeof(struct item));
        req_serv_item -> item_quantity = ordered -> item_quantity;
        req_serv_item -> order_type = REQUEST;
        req_serv_item -> requestedBy = ordered -> requestedBy;
        req_serv_item -> servBy = ordered -> servBy;
        req_serv_item -> next=NULL;

        /*

        // For debugging purposes

        total_order[ordered->requestedBy]++;
        kprintf("%ld ",ordered->requestedBy);

        */
        
        head = req_serv_item;
        ordered++;
        i++;
        V(order_take_full);
    }

    while((head->next)!=NULL)
    {
        head = head->next;
    }

    while(i < N_ITEM_TYPE)
    {
        head -> next = (struct item*) kmalloc(sizeof(struct item));
        head = head -> next;

        head -> item_quantity = ordered -> item_quantity;
        head -> order_type = REQUEST;
        head -> requestedBy = ordered -> requestedBy;
        head -> servBy = ordered -> servBy;
        head -> next = NULL;

        /*

        // For debugging purposes

        total_order[ordered->requestedBy]++;
        kprintf("%ld ",ordered->requestedBy);

        */

        ordered++;
        i++;
        V(order_take_full);
    }

    /*
    
    // For debugging purposes

    for(int i=0;i<NCUSTOMER;i++){
        kprintf("%d ", total_order[i]);
    }

    kprintf("\n");

    */

    V(item_mutex);

}



/**
 * consume_item()
 * Customer consume items which were served by the producers.
 * affected variables in the order queue, on item quantity, order type, requested by, served by
 * customer should keep records for his/her spending in shopping
 * and update spending account
 **/
void consume_item(unsigned long customernum)
{
    // (void) customernum; // avoid warning
    // panic("You need to write some code!!!!");

    P(serv_con_full[customernum]);
    P(item_mutex);

    struct item *head = req_serv_item;

    while(head -> order_type == SERVICED && (unsigned)head-> requestedBy == customernum){
      customer_spending_amount[customernum] += (head->item_quantity) * (head->i_price);
      head = head->next;

      // count_consumed[customernum]++; // For debugging purposes
      
      if(head == NULL) break;
    }

    req_serv_item = head;

    while(head != NULL){
      if((head->next) != NULL && (head->next) -> order_type == SERVICED && (unsigned)(head->next) -> requestedBy == customernum){
        customer_spending_amount[customernum] += ((head->next)->item_quantity) * ((head->next)->i_price);
        head->next = (head->next)->next;
        
        // count_consumed[customernum]++; // For debugging purposes
      
      }
      else head = head->next;
      
    }
    
    /*

    // For debugging purposes

    kprintf("C %ld consumed ordered items for %d times\n",customernum,count_consumed[customernum]);
    for(int i=0;i<NCUSTOMER;i++){
      kprintf("%d ",count_consumed[i]);
    }
    kprintf("\n");

    */


    V(item_mutex);
}



/*
 * end_shoping()
 *
 * This function is called by customers when they go home. It could be
 * used to keep track of the number of remaining customers to allow
 * producer threads to exit when no customers remain.
 */

void end_shoping(){

      //panic("You need to write some code!!!!");
}




/*
 * **********************************************************************
 * FUNCTIONS EXECUTED BY ITEM PRODUCER THREADS
 * **********************************************************************
 */


/*
 * take_order()
 *
 * This function waits for a new order to be submitted by
 * customers. When submitted, it records the details and returns a
 * pointer to something representing the order.
 *
 * The return pointer type is void * to allow freedom of representation
 * of orders.
 *
 * The function can return NULL to signal the producer thread it can now
 * exit as there are no customers nor orders left.
 */
 
void *take_order(){
  //panic("You need to write some code!!!!");

  if(count_order_taken >= (NCUSTOMER * N_ITEM_TYPE * 10))
  {
    V(order_take_full);
    V(item_mutex);
    return NULL;
  }

  P(order_take_full);
  P(item_mutex);
  
  if(count_order_taken >= (NCUSTOMER * N_ITEM_TYPE * 10))
  {
    V(order_take_full);
    V(item_mutex);
    return NULL;
  }

  struct item *head = req_serv_item;
  
  while(head != NULL && head->order_type != REQUEST )
  {
    head = head -> next;
  }

  count_order_taken++;
  head -> order_type = ORDER_TAKEN;
  

  /*

  // For debugging purpose
  
  if(count_order_taken >= 19900){
    kprintf("%d\n",count_order_taken);
  }
  
  */

  V(item_mutex);

  return head; //modify
}


/*
 * produce_item()
 *
 * This function produce an item if the investment is available for the product
 *
 */

void produce_item(void *v){
	(void)v;
    // panic("You need to write some code!!!!");

}



/*
 * serve_order()
 *
 * Takes a produced item and makes it available to the waiting customer.
 */

void serve_order(void *v,unsigned long producernumber){
	//(void)v;
    // (void)producernumber;
    // panic("You need to write some code!!!!");

    P(item_mutex);
    struct item *temp = v;
    
    /*

    // For debugging purposes

    if(temp->order_type == ORDER_TAKEN) No_of_order_taken++;
    if(No_of_order_taken>=19900){
        kprintf("%d\n",No_of_order_taken);
    }

    */

    unsigned int price = ITEM_PRICE + (int)((ITEM_PRICE * (PRODUCT_PROFIT + BANK_INTEREST))/100);
    temp -> order_type = SERVICED;
    temp -> servBy = producernumber;
    temp -> i_price = price;

    /*
    
    // For debugging purposes
    count_served_by[producernumber]++;
    
    */

    count_serve_orders[temp -> requestedBy]++;
    producer_income[producernumber] += (long int)((ITEM_PRICE * temp-> item_quantity * (PRODUCT_PROFIT))/100);

    if(count_serve_orders[temp->requestedBy] >= N_ITEM_TYPE){

        count_serve_orders[temp->requestedBy] = 0;
        V(serv_con_full[temp->requestedBy]);
    }

    V(item_mutex);
    

}



/**
 * calculate_loan_amount()
 * Calculate loan amount
 */
long int calculate_loan_amount(void* itm){
	 //(void)itm;
    // panic("You need to write some code!!!!");

    struct item *temp = itm;
    return (long int)(temp -> item_quantity * ITEM_PRICE);
    
}



/**
 * void loan_request()
 * Request for loan from bank
 */
void loan_request(void *amount,unsigned long producernumber){
	//(void)amount;
    //(void)producernumber;
    // panic("You need to write some code!!!!");

    P(bank_mutex);
    long int *loan = amount;
    long int max=-1,maxind=0;

    for(int i=0;i<NBANK;i++)
    {
      if(bank_account[i].remaining_cash>max)
      {
        maxind = i;
        max = bank_account[i].remaining_cash;
      }
    }
    bank_account[maxind].remaining_cash -= *loan;
    bank_account[maxind].prod_loan[producernumber] += *loan;
    bank_account[maxind].acu_loan_amount += *loan;

    V(bank_mutex);
}

/**
 * loan_reimburse()
 * Return loan amount and service charge
 */
void loan_reimburse(void * loan,unsigned long producernumber){
	//(void)loan;
    //(void)producernumber;
    // panic("You need to write some code!!!!");


    P(bank_mutex);
    
    /*

    // For debugging purposes

    total_loan_reimberse[producernumber]++;
    iterations++;

    */

    long int *amount = loan;
    for(int i = 0; i<NBANK; i++)
    {
      if(bank_account[i].prod_loan[producernumber]== *amount)
      {
        bank_account[i].remaining_cash += *amount;
        bank_account[i].prod_loan[producernumber] -= *amount;
        bank_account[i].interest_amount += (long int)(((*amount)*BANK_INTEREST)/100);
        break;
      }
    }


    /*

    // For debugging purposes

    if(iterations%200 ==0 || iterations == ((NCUSTOMER * N_ITEM_TYPE * 10) - 1) ){
      kprintf("i = %d -> ",iterations);
      for(int i=0;i<NPRODUCER;i++){
        kprintf("%d ",total_loan_reimberse[i]);
      }
      kprintf("\n");

    }*/

    V(bank_mutex);
}



/*
 * **********************************************************************
 * INITIALISATION AND CLEANUP FUNCTIONS
 * **********************************************************************
 */



/*
 * initialize()
 *
 * Perform any initialization you need before opening the investor-producer process to
 * producers and customers
 */

void initialize(){
    //panic("You need to write some code!!!!");

    item_mutex = sem_create("item__mutex", 1);
    order_take_full = sem_create("order_take_full", 0);
    bank_mutex = sem_create("bank_mutex", 1);
    count_order_taken = 0;
  
    for(int i=0;i<NCUSTOMER;i++){
        serv_con_full[i] = sem_create("serv_con_full", 0);
        count_serve_orders[i] = 0;
    }

    /*

    // For debugging purposes 

    for(int i=0;i<NCUSTOMER;i++){
      count_consumed[i] = 0;
      total_order[i] = 0;
    }

    for(int i=0;i<NPRODUCER;i++){
      total_loan_reimberse[i] = 0;
      count_served_by[i] = 0;
    }

    */

}



/*
 * finish()
 *
 * Perform any cleanup investor-producer process after the finish everything and everybody
 * has gone home.
 */

void finish(){
    //panic("You need to write some code!!!!");

    sem_destroy(item_mutex);
    sem_destroy(order_take_full);
    sem_destroy(bank_mutex);
    for(int i =0; i<NCUSTOMER; i++)
    {
        sem_destroy(serv_con_full[i]);    
    }
}
