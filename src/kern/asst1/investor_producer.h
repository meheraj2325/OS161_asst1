/*
 * **********************************************************************
 * You are free to add or modify anything you think you require to this file
 */

#ifndef _INVESTOR_PRODUCER_H_
#define _INVESTOR_PRODUCER_H_

/**
 * FUNCTIONs Prototype declares here
 * Add or modify more funtion if you need more
 **/
void order_item(void *); 			// customer orders one or more items
void consume_item(unsigned long); 			// consume items and update spending
void end_shoping(void); 			//finish shoping and go home


/* Producer functions */
void *take_order(void); 			//take order from the customers
void produce_item(void *); 			// producer produce an item or product
long int calculate_loan_amount(void*); 		//calculate loan amount
void loan_request(void *,unsigned long); 			// producer request loan to a bank
void serve_order(void *,unsigned long); 			// producer serve the request of a customer
void loan_reimburse(void *,unsigned long); 			// return loan money with service charge
void* deleteItem(unsigned long customernum, void* it);

/* Investor-Producer process opening and closing functions */

void initialize(void);
void finish(void);

extern struct item *req_serv_item;
extern struct bankdata bank_account[NBANK];
extern long int customer_spending_amount[NCUSTOMER];
extern long int producer_income[NPRODUCER];

#endif
