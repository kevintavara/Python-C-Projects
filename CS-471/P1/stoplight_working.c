/*
 *
 * Includes
 *
 */

#include <types.h>
#include <lib.h>
#include <test.h>
#include <thread.h>
#include <synch.h>
#include <queue.h>

/*
 *
 * Constants
 *
 */

/*
 * Number of vehicles created.
 */

#define NVEHICLES 20

struct lock* AB, * BC, * CA;


/*
 *
 * Function Definitions
 *
 */





static void turnLProc(char dir, char dest, unsigned long vehiclenumber, char* vehicletype, struct lock *lock, struct lock *lock2) {
        lock_acquire(lock);
        kprintf("%s %d is turning left from %c to %c, it is currently in %s\n",vehicletype, (int)vehiclenumber, dir, dest, lock->name);
        lock_acquire(lock2);
        kprintf("%s %d has gone from %s to %s\n",vehicletype, (int)vehiclenumber, lock->name, lock2->name);
        lock_release(lock);
        lock_release(lock2);
        kprintf("%s %d has left %s and is no longer in the intersection.\n",vehicletype, (int)vehiclenumber, lock2->name);
}

/*
 * turnleft()
 *
 * Arguments:
 *      unsigned long vehicledirection: the direction from which the vehicle
 *              approaches the intersection.
 *      unsigned long vehiclenumber: the vehicle id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a left turn through the
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
turnleft(unsigned long vehicledirection,
                unsigned long vehiclenumber,
                unsigned long vehicletype)
{
        switch(vehicledirection) {
                case 0:
                        if (vehicletype == 0) {
                                kprintf("Car %d is approaching the intersection frome lane A\n", (int)vehiclenumber);
                                while(lock_do_i_hold(AB) == 1 || lock_do_i_hold(BC) == 1);
                                turnLProc('A','C',vehiclenumber,"Car",AB,BC);
                                break;
                        }
                        else {
                                kprintf("Truck %d is approaching the intersection frome lane A\n", (int)vehiclenumber);
                                while(lock_do_i_hold(CA) == 1 || lock_do_i_hold(BC) == 1 || lock_do_i_hold(AB) == 1);
                                while(!q_empty(AB->q));
                                turnLProc('A','C',vehiclenumber,"Truck",AB,BC);
                                break;
                        }
                case 1:
                        if (vehicletype == 0) {
                                kprintf("Car %d is approaching the intersection frome lane B\n", (int)vehiclenumber);
                                while(lock_do_i_hold(CA) == 1 || lock_do_i_hold(BC) == 1);
                                turnLProc('B','A',vehiclenumber,"Car",BC,CA);
                                break;
                        }
                        else {
                                kprintf("Truck %d is approaching the intersection frome lane B\n", (int)vehiclenumber);
                                while(lock_do_i_hold(AB) == 1 || lock_do_i_hold(CA) == 1 || lock_do_i_hold(BC) == 1);
                                while(!q_empty(BC->q));
                                turnLProc('B','A',vehiclenumber,"Truck",BC,CA);
                                break;
                        }
                case 2:
                        if (vehicletype == 0) {
                                kprintf("Car %d is approaching the intersection frome lane C\n", (int)vehiclenumber);
                                while(lock_do_i_hold(AB) == 1 || lock_do_i_hold(CA) == 1);
                                turnLProc('C','B',vehiclenumber,"Car",CA,AB);
                                break;
                        }
                        else {
                                kprintf("Truck %d is approaching the intersection frome lane C\n", (int)vehiclenumber);
                                while(lock_do_i_hold(BC) == 1 || lock_do_i_hold(CA) == 1 || lock_do_i_hold(AB) == 1);
                                while(!q_empty(CA->q));
                                turnLProc('C','B',vehiclenumber,"Truck",CA,AB);
                                break;
                        }
                }

}


static void turnRProc(char dir, char dest, unsigned long vehiclenumber, char* vehicletype, struct lock *lock) {
        if (vehicletype[0] == 'C')
                kprintf("%s %d is approaching the intersection from lane %c\n", vehicletype, (int)vehiclenumber, dir);
        lock_acquire(lock);
        kprintf("%s %d is turning right from %c to %c, it is currently in %s\n", vehicletype, (int)vehiclenumber, dir, dest, lock->name);
        lock_release(lock);
        kprintf("%s %d has left %s and is no longer in the intersection\n",vehicletype, (int)vehiclenumber, lock->name);

}



        /*
         * Avoid unused variable warnings.
         */

/*      (void) vehicledirection;
        (void) vehiclenumber;
        (void) vehicletype;
*/


/*
 * turnright()
 *
 * Arguments:
 *      unsigned long vehicledirection: the direction from which the vehicle
 *              approaches the intersection.
 *      unsigned long vehiclenumber: the vehicle id number for printing purposes.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a right turn through the
 *      intersection from any direction.
 *      Write and comment this function.
 */




static
void
turnright(unsigned long vehicledirection,
                unsigned long vehiclenumber,
                unsigned long vehicletype)
{
        switch (vehicledirection) {

        case 0:
                if (vehicletype == 0) {
                        turnRProc('A','B',vehiclenumber,"Car",AB);
                        break;
                }
                else {
                        kprintf("Truck %d is approaching the intersection frome lane A\n", (int)vehiclenumber);
                        while (lock_do_i_hold(BC) == 1 || lock_do_i_hold(CA) == 1);
                        while(!q_empty(AB->q));
                        turnRProc('A','B',vehiclenumber,"Truck",AB);
                        break;
                }
        case 1:
                if (vehicletype == 0) {
                        turnRProc('B','C',vehiclenumber,"Car",BC);
                        break;
                }
                else {
                        kprintf("Truck %d is approaching the intersection frome lane B\n", (int)vehiclenumber);
                        while (lock_do_i_hold(AB) == 1 || lock_do_i_hold(CA) == 1);
                        while(!q_empty(BC->q));
                        turnRProc('B','C',vehiclenumber,"Truck",BC);
                        break;
                }
        case 2:
                if (vehicletype == 0) {
                        turnRProc('C','A',vehiclenumber,"Car",CA);
                        break;
                }
                else {
                        kprintf("Truck %d is approaching the intersection frome lane C\n", (int)vehiclenumber);
                        while (lock_do_i_hold(AB) == 1 || lock_do_i_hold(BC) == 1);
                        while(!q_empty(CA->q));
                        turnRProc('C','A',vehiclenumber,"Truck",CA);
                        break;
                }
        }

}


/*
 * approachintersection()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long vehiclenumber: holds vehicle id number.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Change this function as necessary to implement your solution. These
 *      threads are created by createvehicles().  Each one must choose a direction
 *      randomly, approach the intersection, choose a turn randomly, and then
 *      complete that turn.  The code to choose a direction randomly is
 *      provided, the rest is left to you to implement.  Making a turn
 *      or going straight should be done by calling one of the functions
 *      above.
 */

static
void
approachintersection(void * unusedpointer,
                unsigned long vehiclenumber)
{
        int vehicledirection, turndirection, vehicletype;

        /*
         * Avoid unused variable and function warnings.
         */

//      int spl;

        (void)unusedpointer;
        (void)turnright;
        (void)turnleft;

        vehicledirection = random() % 3;
        turndirection = random() % 2;
        vehicletype = random() % 2;
//      kprintf("Vehicle %lu is approaching the intersection.\n", vehiclenumber);
        if (turndirection == 0) {
                turnright(vehicledirection, vehiclenumber, vehicletype);

        }
        else if (turndirection == 1) {
                turnleft(vehicledirection, vehiclenumber, vehicletype);
        }

}
        /*
         * vehicledirection is set randomly.
         */




/*
 * createvehicles()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up the approachintersection() threads.  You are
 *      free to modiy this code as necessary for your solution.
 */

int
createvehicles(int nargs,
                char ** args)
{
        int index, error;

        /*
         * Avoid unused variable warnings.
         */

        (void) nargs;
        (void) args;

        /*
         * Start NVEHICLES approachintersection() threads.
         */
        AB = lock_create("AB");
        BC = lock_create("BC");
        CA = lock_create("CA");
        for (index = 0; index < NVEHICLES; index++) {

                error = thread_fork("approachintersection thread",
                                NULL,
                                index,
                                approachintersection,
                                NULL
                                );

                /*
                 * panic() on error.
                 */

                if (error) {

                        panic("approachintersection: thread_fork failed: %s\n",
                                        strerror(error)
                                 );
                }
        }

        return 0;
}