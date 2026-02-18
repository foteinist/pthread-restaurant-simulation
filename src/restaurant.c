#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "config.h"

// Mutex and Condition Variables for Call Handling
pthread_mutex_t lock_Ntel;
pthread_cond_t cond_Ntel;

// Mutex and Condition Variables for Cooking
pthread_mutex_t lock_Ncook;
pthread_cond_t cond_Ncook;

// Mutex and Condition Variables for Oven Availability
pthread_mutex_t lock_Noven;
pthread_cond_t cond_Noven;

// Mutex and Condition Variables for Deliverer Availability
pthread_mutex_t lock_Ndeliverer;
pthread_cond_t cond_Ndeliverer;

// Mutex for Revenue Tracking
pthread_mutex_t revenue_lock;

// Mutex for Statistics Tracking
pthread_mutex_t lock_statistics;

// Mutex for Output Synchronization
pthread_mutex_t lock_output;

// Seed for Random Number Generation
int seed;

// Global variables for the restaurant's revenue and statistics
int store_revenue=0;
int count_pizza_margarita=0;
int count_pizza_special=0;
int count_pizza_peperoni=0;
int count_fails=0;
int count_success=0;
double total_service_time=0.0;
double max_service_time=0.0;
double total_cooling_time=0.0;
double max_cooling_time=0.0;

// Struct to represent a customer
typedef struct customer{
    int id;
    int random;
} customer_id;


// Function declarations
void *prepare_pizzas(void *threadId, int numPizzas, struct timespec *start);
void *bake_pizzas(void *threadId, int numPizzas, struct timespec *start);
void *deliver_order(void *threadId, int numPizzas, struct timespec *start,struct timespec *start2);

// Update service time statistics
void update_statistics(double service_time){
    pthread_mutex_lock(&lock_statistics);
    total_service_time+=service_time;
    if(service_time>max_service_time){
        max_service_time=service_time;
    }
    pthread_mutex_unlock(&lock_statistics);
}

// Update cooling time statistics
void update_cooling_statistics(double cooling_time){
    pthread_mutex_lock(&lock_statistics);
    total_cooling_time+=cooling_time;
    if(cooling_time>max_cooling_time){
        max_cooling_time=cooling_time;
    }
    pthread_mutex_unlock(&lock_statistics);
}

void *answers_calls_worker(void *threadId){
   
    struct timespec start;

    // Variables to keep track of different types of pizzas
    int numberOfMargaritaPizza=0;
    int numberOfPeperoniPizza=0;
    int numberOfSpecialPizza=0;
    int customer_pay;

    // Extract customer ID from thread ID
    customer_id *tid=(customer_id*)threadId;
    int mutex_Ntel;
    clock_gettime(CLOCK_REALTIME, &start);

    pthread_mutex_lock(&lock_output);
    printf("Ring Ring!\n");
    pthread_mutex_unlock(&lock_output);

    // Lock to handle the call
    mutex_Ntel=pthread_mutex_lock(&lock_Ntel);
    if(mutex_Ntel!=0){
        pthread_mutex_lock(&lock_output);
        printf("ERROR: return code from pthread_mutex_lock() is %d\n",mutex_Ntel);
        pthread_mutex_unlock(&lock_output);
        pthread_exit(&mutex_Ntel);
    }

    // Check if operator is busy, wait if necessary
    while(Ntel==0){
        pthread_mutex_lock(&lock_output);
        printf("Operator is Busy!The costumer %d is waiting\n",tid->id);
        pthread_mutex_unlock(&lock_output);
        mutex_Ntel=pthread_cond_wait(&cond_Ntel,&lock_Ntel);
    }
    Ntel--;
    pthread_mutex_lock(&lock_output);
    printf("The costumer %d communicates with the worker!\n",tid->id);
    pthread_mutex_unlock(&lock_output);
    mutex_Ntel=pthread_mutex_unlock(&lock_Ntel);

    // Determine the number of pizzas ordered
    int randomNumberOfPizzas=rand_r(&(tid->random))%Norderhigh + Norderlow;

    //printf("number of pizzas %d! \n",randomNumberOfPizzas);
    int i;
    for(i=0; i<randomNumberOfPizzas; i++){
        if(rand_r(&(tid->random))%100<=Pm*100){
            //printf("                     Margarita\n");
            numberOfMargaritaPizza++;
        }
         else if(rand_r(&(tid->random))%100<=Pp*100){
            //printf("                     Peperoni\n");
            numberOfPeperoniPizza++;
        }else{
            //printf("                     Special\n");
            numberOfSpecialPizza++;
        }
    }

    // Calculate the total price for the order
    customer_pay= numberOfMargaritaPizza*Cm + numberOfPeperoniPizza*Cp+ numberOfSpecialPizza*Cs;
    pthread_mutex_lock(&lock_output);
    printf("The costumer with id %d has to pay %d!\n",tid->id,customer_pay);
    pthread_mutex_unlock(&lock_output);

    // Simulate the time taken for payment
    int randomTimePayment=rand_r(&(tid->random))%Tpaymenthigh + Tpaymentlow;
    sleep(randomTimePayment);
    pthread_mutex_lock(&lock_output);
    //printf("Propability is %d !\n",rand_r(&(tid->random))%100);
    pthread_mutex_unlock(&lock_output);
    if(rand_r(&(tid->random))%100<=Pfail*100){
        // If the order fails
        pthread_mutex_lock(&lock_output);
        printf("The order %d has failed!\n", tid->id);
        pthread_mutex_unlock(&lock_output);
        count_fails++;
        mutex_Ntel=pthread_mutex_lock(&lock_Ntel);
        Ntel++;
        mutex_Ntel=pthread_cond_signal(&cond_Ntel);

        mutex_Ntel=pthread_mutex_unlock(&lock_Ntel);
        pthread_mutex_lock(&lock_output);
        pthread_mutex_unlock(&lock_output);
        pthread_exit(threadId);
    }else{
        // If the order is successful
        count_success++;
        //printf("The order was a success!\n");
        store_revenue+=customer_pay;
        //printf("The store revenue %d!\n",store_revenue);
        count_pizza_margarita+=numberOfMargaritaPizza;
        //printf("Total pizza_margarita %d\n",count_pizza_margarita);
        count_pizza_peperoni+=numberOfPeperoniPizza;
        //printf("pizza_peperoni %d\n",count_pizza_peperoni);
        count_pizza_special+=numberOfSpecialPizza;
        //printf("pizza_special %d\n",count_pizza_special);
    }
    if(mutex_Ntel!=0){
        pthread_mutex_lock(&lock_output);
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n",mutex_Ntel);
        pthread_mutex_unlock(&lock_output);
        pthread_exit(&mutex_Ntel);
    }
    pthread_mutex_lock(&lock_output);
    printf("The phonecall with %d was a success!\n",tid->id);
    pthread_mutex_unlock(&lock_output);
    mutex_Ntel=pthread_mutex_lock(&lock_Ntel);
    Ntel++;
    mutex_Ntel=pthread_cond_signal(&cond_Ntel);
    mutex_Ntel=pthread_mutex_unlock(&lock_Ntel);

    // Proceed to prepare the pizzas
    prepare_pizzas(threadId, numberOfMargaritaPizza + numberOfPeperoniPizza + numberOfSpecialPizza, &start);

    // Exit the thread
    pthread_exit(threadId);
}

void *prepare_pizzas(void *threadId, int numPizzas, struct timespec *start) {

    customer_id *tid=(customer_id*)threadId;
    int mutex_Ncook;

    // Lock the cook mutex to check and possibly wait for an available cook
    mutex_Ncook = pthread_mutex_lock(&lock_Ncook);

    if(mutex_Ncook!=0){
        pthread_mutex_lock(&lock_output);
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n",mutex_Ncook);
        pthread_mutex_unlock(&lock_output);
        pthread_exit(&mutex_Ncook);
    }

    // Wait if no cooks are available
    while (Ncook == 0) {
        pthread_mutex_lock(&lock_output);
        printf("No pizza maker available.Customer %d is waiting.\n", tid->id);
        pthread_mutex_unlock(&lock_output);
        mutex_Ncook = pthread_cond_wait(&cond_Ncook, &lock_Ncook);
    }

    // A cook is available, decrease the count of available cooks
    pthread_mutex_lock(&lock_output);
    printf("Pizza maker is preparing %d pizzas for customer %d.\n", numPizzas, tid->id);
    pthread_mutex_unlock(&lock_output);
    Ncook--;
    mutex_Ncook=pthread_mutex_unlock(&lock_Ncook);

    // Simulate the time taken to prepare the pizzas
    sleep(numPizzas*Tprep);
   
    pthread_mutex_lock(&lock_output);
    printf("Preparation for order %d is done!\n", tid->id);
    pthread_mutex_unlock(&lock_output);
   
    if(mutex_Ncook!=0){
        pthread_mutex_lock(&lock_output);
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n",mutex_Ncook);
        pthread_mutex_unlock(&lock_output);
        pthread_exit(&mutex_Ncook);
    }
   
    int mutex_Noven;
    // Lock the oven mutex to check and possibly wait for available ovens
    mutex_Noven = pthread_mutex_lock(&lock_Noven);

    if(mutex_Noven!=0){
        pthread_mutex_lock(&lock_output);
        printf("ERROR: return code from pthread_mutex_lock() is %d\n",mutex_Noven);
        pthread_mutex_unlock(&lock_output);
        pthread_exit(&mutex_Noven);
    }

    // Wait if not enough ovens are available for the number of pizzas
    while (Noven < numPizzas) {
        pthread_mutex_lock(&lock_output);
        printf("Not enough ovens available. Order %d is waiting.\n", tid->id);
        pthread_mutex_unlock(&lock_output);
        mutex_Noven = pthread_cond_wait(&cond_Noven, &lock_Noven);
    }

    // Enough ovens are available, decrease the count of available ovens
    Noven -= numPizzas;
    mutex_Noven=pthread_mutex_unlock(&lock_Noven);
    if(mutex_Noven!=0){
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n",mutex_Noven);
        pthread_exit(&mutex_Noven);
    }

    // Unlock the cook mutex and signal that a cook has become available
    mutex_Ncook=pthread_mutex_lock(&lock_Ncook);

    Ncook++;
    mutex_Ncook=pthread_cond_signal(&cond_Ncook);
    mutex_Ncook=pthread_mutex_unlock(&lock_Ncook);

    // Proceed to bake the pizzas
    bake_pizzas(threadId, numPizzas,start);
}

void *bake_pizzas(void *threadId, int numPizzas,struct timespec *start) {

    customer_id *tid=(customer_id*)threadId;
    struct timespec start2;

    // Log that baking has started for the order
    pthread_mutex_lock(&lock_output);
    printf("Cook is baking %d pizzas for order %d.\n", numPizzas, tid->id);
    pthread_mutex_unlock(&lock_output);

    // Simulate the time taken to bake the pizzas
    sleep(Tbake);

    // Log that baking has completed for the order
    pthread_mutex_lock(&lock_output);
    printf("Baking completed for order %d.\n", tid->id);
    pthread_mutex_unlock(&lock_output);
    clock_gettime(CLOCK_REALTIME, &start2);
    int mutex_Ndeliverer;

    // Lock the deliverer mutex to check and possibly wait for an available deliverer
    mutex_Ndeliverer = pthread_mutex_lock(&lock_Ndeliverer);

    if(mutex_Ndeliverer!=0){
        pthread_mutex_lock(&lock_output);
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n",mutex_Ndeliverer);
        pthread_mutex_unlock(&lock_output);
        pthread_exit(&mutex_Ndeliverer);
    }

    // Wait if no deliverers are available
    while (Ndeliverer == 0) {
        pthread_mutex_lock(&lock_output);
        printf("No deliverer available. Order %d is waiting.\n", tid->id);
        pthread_mutex_unlock(&lock_output);
        mutex_Ndeliverer = pthread_cond_wait(&cond_Ndeliverer, &lock_Ndeliverer);
    }

    // A deliverer is available, decrease the count of available deliverers
    Ndeliverer--;
    mutex_Ndeliverer = pthread_mutex_unlock(&lock_Ndeliverer);
    if(mutex_Ndeliverer!=0){
        pthread_mutex_lock(&lock_output);
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n",mutex_Ndeliverer);
        pthread_mutex_unlock(&lock_output);
        pthread_exit(&mutex_Ndeliverer);
    }

    // Lock the oven mutex to increase the count of available ovens
    pthread_mutex_lock(&lock_Noven);
   
    Noven += numPizzas;
    pthread_cond_signal(&cond_Noven); // Signal other threads waiting for ovens
    pthread_mutex_unlock(&lock_Noven);

    // Proceed to deliver the order
    deliver_order(threadId, numPizzas, start, &start2);
}

void *deliver_order(void *threadId, int numPizzas,struct timespec *start,struct timespec *start2) {

    struct timespec end;
    struct timespec end2;
    customer_id *tid=(customer_id*)threadId;

    // Log that the deliverer is packing the order
    pthread_mutex_lock(&lock_output);
    printf("Deliverer is packing order %d.\n", tid->id);
    pthread_mutex_unlock(&lock_output);

    // Simulate the time taken to pack the order
    sleep(numPizzas*Tpack);
    clock_gettime(CLOCK_REALTIME, &end);
    double elapsed_time_minutes = end.tv_sec - start->tv_sec;

    // Log the packing time
    pthread_mutex_lock(&lock_output);
    printf("Order %d was packed in %.f minutes.\n", tid->id, elapsed_time_minutes);
    pthread_mutex_unlock(&lock_output);

    // Calculate a random delivery time within the specified range
    int deliveryTime =rand_r(&(tid->random)) % (Tdelhigh - Tdellow + 1) + Tdellow;

    // Log that the deliverer is delivering the order
    pthread_mutex_lock(&lock_output);
    printf("Deliverer is delivering order %d.\n", tid->id);
    pthread_mutex_unlock(&lock_output);

    // Simulate the time taken to deliver the order
    sleep(deliveryTime);
    clock_gettime(CLOCK_REALTIME, &end2);
    double cooling_time_minutes = end2.tv_sec - start2->tv_sec; // Calculate the cooling time
    double delivery_time_minutes = end2.tv_sec - start->tv_sec; // Calculate the total delivery time
    update_statistics(delivery_time_minutes);
    pthread_mutex_lock(&lock_output);
    printf("Total delivery time for order %d: %.f minutes.\n", tid->id, delivery_time_minutes);
    pthread_mutex_unlock(&lock_output);
   
    update_cooling_statistics(cooling_time_minutes);

    // Simulate the return time for the deliverer
    sleep(deliveryTime);
    //printf("Total time the deliverer was busy: %u minutes.\n", 2*deliveryTime);

    // Lock the deliverer mutex to update the count of available deliverers
    pthread_mutex_lock(&lock_Ndeliverer);
    Ndeliverer++; // Increment the count of available deliverers
    pthread_cond_signal(&cond_Ndeliverer); // Signal other threads waiting for a deliverer
    pthread_mutex_unlock(&lock_Ndeliverer);
}

int main(int argc, char *argv[]){

    // Check if the correct number of arguments are provided
    int Ncust;
    if (argc != 3){
        printf("ERROR: the program should take two arguments,the number of customers to be served and a random seed!\n");
        exit(-1);
    }

    // Convert arguments to integers
    Ncust=atoi(argv[1]);
    seed=atoi(argv[2]);

    // Check if arguments are valid
    if(Ncust<0 && seed<0){
        printf("ERROR:Both arguments are wrong!\n");
        return -1;
    }

    if(Ncust<0){
        printf("ERROR:The number of customers is negative!Please give a positive number\n");
        return -1;
    }else{
        printf("The number of customers is %d! \n",Ncust);
    }

    if(seed<0){
        printf("ERROR:The seed is negative! Please give a positive number\n");
        return -1;
    }

    // Initialize mutex and condition variables
    int x=pthread_mutex_init(&lock_Ntel, NULL);
    if (x != 0){
        printf("ERROR: return code from pthread_mutex_init() is %d\n", x);
        exit(-1);
    }
    x= pthread_cond_init(&cond_Ntel, NULL);
    if (x != 0){
        printf("ERROR: return code from pthread_cond_init() is %d\n", x);
        exit(-1);
    }
   
    // Seed the random number generator
    srand(time(NULL));

    // Allocate memory for threads
    pthread_t *threads;
    // Create array
    threads = malloc(Ncust * sizeof(pthread_t));
    if (threads == NULL)
    {
    printf("NOT ENOUGH MEMORY!\n");
    return -1;
    }

    int rc;
    int threadCount;
    customer_id Customers_Id[Ncust];

    // Create threads and initialize customer IDs
    for(threadCount = 0; threadCount < Ncust; threadCount++){
        printf("Main:Costumers id %d\n", threadCount+1);
        Customers_Id[threadCount].id= threadCount + 1;
        Customers_Id[threadCount].random= seed + threadCount+1;
        // Create thread for each customer
        rc = pthread_create(&threads[threadCount], NULL, answers_calls_worker, &Customers_Id[threadCount]);

        // Check if thread creation was successful
        if (rc != 0){
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
        // Generate a random waiting time before the next call
        int waiting_new_call= rand_r(&seed)% Torderhigh+Torderlow;
        sleep(waiting_new_call);
    }
   
    // Join threads
    void *status;
    for (threadCount = 0; threadCount < Ncust; threadCount++){
        rc = pthread_join(threads[threadCount], &status);
        if (rc != 0){
            printf("ERROR: return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
        printf("Main: Thread %d returned %d as status code.\n", threadCount+1, (*(int *)status));
    }

    // Print statistics
    printf("Total revenue: %d!\n",store_revenue);
    printf("Number of pizza margarita: %d!\n",count_pizza_margarita);
    printf("Number of pizza peperoni: %d!\n",count_pizza_peperoni);
    printf("Number of pizza special: %d!\n",count_pizza_special);
    printf("Number of successful orders: %d!\n",count_success);
    printf("Number of failed orders: %d!\n",count_fails);

    // Print average and maximum service time and cooling time if there are successful orders
    if(count_success>0){
        double avg_service_time=total_service_time/count_success;
        printf("Average customer service time for successful orders: %.f minutes\n", avg_service_time);
        printf("Maximum customer service time for successful orders: %.f minutes\n", max_service_time);
        double avg_cooling_time=total_cooling_time/count_success;
        printf("Average cooling time for successful orders: %.f minutes\n", avg_cooling_time);
        printf("Maximum cooling time for successful orders: %.f minutes\n", max_cooling_time);
    }else{
        printf("We don't have successful orders!");
        return -1;
    }

    // Free allocated memory for threads
    free(threads);

    // Destroy mutexes and condition variables
    pthread_mutex_destroy(&lock_Ntel);
    pthread_cond_destroy(&cond_Ntel);

    pthread_mutex_destroy(&lock_Ncook);
    pthread_cond_destroy(&cond_Ncook);

    pthread_mutex_destroy(&lock_Noven);
    pthread_cond_destroy(&cond_Noven);

    pthread_mutex_destroy(&lock_Ndeliverer);
    pthread_cond_destroy(&cond_Ndeliverer);

    pthread_mutex_destroy(&revenue_lock);

    pthread_mutex_destroy(&lock_output);

    pthread_mutex_destroy(&lock_statistics);

    return 1;
}
