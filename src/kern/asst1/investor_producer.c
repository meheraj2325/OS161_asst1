#include <types.h>
#include <lib.h>
#include <synch.h>
#include <test.h>
#include <thread.h>

#include "invest_assignment.h"
#include "investor_producer.h"


static struct semaphore *item_mutex;
static struct semaphore *item_full;
static struct semaphore *bank_mutex;


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
    (void)itm; // to avoid warning
    //panic("You need to write some code!!!!");

    P(item_mutex);

    struct item *ordered = itm;
    struct item *head = req_serv_item;
    int i = 0;


    if(head==NULL)
    {
        req_serv_item = (struct item*) kmalloc(sizeof(struct item));
        req_serv_item -> item_quantity = ordered -> item_quantity;
        // req_serv_item -> i_price = (ordered -> i_price == NULL) ? ITEM_PRICE : ordered -> i_price;
        req_serv_item -> order_type = REQUEST;
        req_serv_item -> requestedBy = ordered -> requestedBy;
        req_serv_item -> servBy = ordered -> servBy;
        req_serv_item -> next=NULL;

        head = req_serv_item;
        ordered++;
        i++;
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
       // head -> i_price = (ordered -> i_price == NULL) ? ITEM_PRICE : ordered -> i_price;
        head -> order_type = REQUEST;
        head -> requestedBy = ordered -> requestedBy;
        head -> servBy = ordered -> servBy;
        head -> next = NULL;

        ordered++;
        i++;
    }

    V(item_mutex);
    V(item_full);

}


void* deleteItem(unsigned long customernum, void* It)
{
    struct item* it = It;
    if(it == NULL)return it;
    if(it -> order_type == SERVICED && (unsigned)it-> requestedBy == customernum)
    {
        customer_spending_amount[customernum] += (it->item_quantity) * (it->i_price);
        it = it -> next;
    }
    else {
     it->next = deleteItem(customernum, it->next);
     return it;
    }
    return it = deleteItem(customernum, it);
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
    (void) customernum; // avoid warning
    //panic("You need to write some code!!!!");
    P(item_full);
    P(item_mutex);

    req_serv_item = deleteItem(customernum,req_serv_item);

    V(item_mutex);
}

/*
 * end_shoping()
 *
 * This function is called by customers when they go home. It could be
 * used to keep track of the number of remaining customers to allow
 * producer threads to exit when no customers remain.
 */

void end_shoping()
{
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

void *take_order()
{
    // panic("You need to write some code!!!!");

    P(item_full);
    P(item_mutex);
    struct item *head = req_serv_item;

    
    while(head->order_type != REQUEST)
    {
      head = head -> next;
    }

    V(item_mutex);


    return head; //modify
}


/*
 * produce_item()
 *
 * This function produce an item if the investment is available for the product
 *
 */

void produce_item(void *v)
{
    (void)v;
    // panic("You need to write some code!!!!");

    P(item_mutex);

    struct item *head = v;
    head -> order_type = SERVICED;

    V(item_mutex);
}


/*
 * serve_order()
 *
 * Takes a produced item and makes it available to the waiting customer.
 */

void serve_order(void *v,unsigned long producernumber)
{
    (void)v;
    (void)producernumber;
    // panic("You need to write some code!!!!");

    P(item_mutex);

    struct item *temp = v;
    unsigned int price = ITEM_PRICE + (ITEM_PRICE * (int)((PRODUCT_PROFIT + BANK_INTEREST)/100));
    temp -> i_price = price;

    V(item_mutex);
    V(item_full);
}

/**
 * calculate_loan_amount()
 * Calculate loan amount
 */
long int calculate_loan_amount(void* itm)
{
    (void)itm;
    // panic("You need to write some code!!!!");
    P(item_mutex);
    struct item *temp = itm;
    return (long int)(temp -> item_quantity * ITEM_PRICE);
    V(item_mutex);
}

/**
 * void loan_request()
 * Request for loan from bank
 */
void loan_request(void *amount,unsigned long producernumber)
{
    (void)amount;
    (void)producernumber;
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
void loan_reimburse(void * loan,unsigned long producernumber)
{
    (void)loan;
    (void)producernumber;
    // panic("You need to write some code!!!!");

    P(bank_mutex);

    long int *amount = loan;
    for(int i = 0; i<NBANK; i++)
    {
      if(bank_account[i].prod_loan[producernumber]== *amount)
      {
        bank_account[i].remaining_cash += *amount;
        bank_account[i].prod_loan[producernumber] -= *amount;
        bank_account[i].acu_loan_amount -= *amount;
        bank_account[i].interest_amount += (*amount)*(long int)(BANK_INTEREST/100);
        break;
      }
    }
    P(bank_mutex);
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

void initialize()
{
  //panic("You need to write some code!!!!");
  item_mutex = sem_create("item_list_mutex", 1);
  bank_mutex = sem_create("bank_mutex", 1);
  item_full = sem_create("item_list_full", 0);
}

/*
 * finish()
 *
 * Perform any cleanup investor-producer process after the finish everything and everybody
 * has gone home.
 */

void finish()
{
    //panic("You need to write some code!!!!");
    sem_destroy(item_mutex);
    sem_destroy(item_full);
}
