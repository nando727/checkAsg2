// Fernando Tello

/* implement a simulation of two scheduling policies on a single CPU
    * priority based ie non preempitive
    * custom ie preemptive
    * 
 * the policy to simulate should be select by command line arg
    * -prio (priority)
    * -custom (custom)
 * output written to file named output.data
*/


/* priority based details
    * every 'clock tick', the process w/ the highest priority number
    (ie lowest int) will be selected & executed non preemptively

    * if mutliple processes have the same priority number, then
    the one w/ the shortest service time will be given execution priority
    
    * if both priority & service time equal, just arbitrarily pick the 
    first process to arrive

*/

/* custom details, ie preemptive (a variation of priority based)
    * ever 'clock tick' a process w/ the highest priority (ie lowest int)
    will be select ot run and will run for 1 'time quantum' (ie clock tick)

    * its priority will be lowered by one (increment integer) and placed back
    on 'ready queue'

    * higher integer = lower priority

    * if several proc. have same priority, proc. w/ the shortest remaining service time,
    the proc. w/ the shortest remaining service time will be selected to run next

    * if priority & service time are the same, pick the process that arrived first
*/

/* input file
    * read file input redirection <
    * 1st number of each line is the arrival time of a task
    * 2nd number is the service time of that task
    * 3rd number is the priority of the process

    * input groups will be ordered according to ascending arrival times,
    multiple tasks can arrive at the same time
*/

/* more details
    * each scheudling algorithm should assign alphabetic task identifiers to
    each task in ascending order
        * start w/ tid Y 
    * assume no more than 26 tasks
    * when task ids reach the end of the alphabet, roll over to the beginning
    of the alphabet

    * note that there may be idle times prior to the first task arriving,
    idle times between groups of tasks, as weel as tasks arriving at the same time
*/

/* printing to the output file
    * a trace of each time unit, labeled as the 'discrete time value' at the
    beginning of the time unit
        * AND show what task is running on the cpu
        * AND what tasks are waiting in the 'ready queue'

    * any arrivals, preemption, and scheduling occur at the 'discrete time value'
    that starts a timeunit and icur zero overhead
    * completions occur at the discrete time value that ends a time unit
*/

/* omg more details
    * during the trace, the tasks hould be identified as the tid 'concatenated w/ rst',
    where tid is the task identifier assigned upon arrival
        * and 'rst' wis the current remaining service time evalutated at the start of a time unit
    
    * you should stop the sim. after all tasks have completed
        * NOTE: this means the CPU is empty, the ready queue is empty, 
        and there is no more input
    
    * note that for custom a running task is preempted only 
    when an arriving task has less service time

    * after the sim. ends, print (via output file redirection) a 'task summary table'
        * ordered by ascending task indetifier
        * containing the tid, arrival tme, service time, completuon time,
        response time, and wait time for each task.
    
    * finally, print to the same file a table of service time and wait time pairs 
    from the 'task summary table', ordered by ascedning service time

    * use a linked list w/ pointers, w/ each task being represented by a pointer
    to Task struct.
    * you can ONlY use this pointer via another pointer ie double pointer, 
    mame that pointer as you wish
*/


/* second policy output:
Custom(preemptive) scheduling results
time cpu priority ready queue (tid/rst)
---------------------------------
0                           --
1                           --
2                           --
3       Y4          7       --
4       Y3          8       --
5       Y2          9       --
6       Y1          10      --
7                           --
8                           --
9                           --
10      Z4          6       A2 8
11      Z3          7       A2 8
12      Z2          8       A2 8
13      A2          8       Z1 9
14      A1          9       Z1 9
15      Z1          9       --

                arrival     service     completion  response   wait
tid     prio    time        time        time        time       time
-------------------------------------------------------------------
A       8       10          2           15          x           x (calculate time here)
Y       7       3           4           7           x           x
Z       6       10          4           16          x           x


service wait
time    time
------- ----
2       4
4       0
4       0
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct task {
    int task_id,
        arrival_time,
        service_time,
        remaining_time,
        completion_time,
        response_time,
        wait_time,
        priority;
    struct task *next;
} Task;

Task* pop_task(Task** task_list) {
    // point 'first' to the current first task (ie the one being removed) & 
    // update the head of the list to point to the second task
    Task* first = *task_list;
    *task_list = first->next;

    // remove the 'next' ptr from the 'first' being popped ie no longer apart of the list
    // and doesn't point to the next task in the list
    first->next = NULL;
    return first;
}

void push_task(Task** task_list, Task* new_task) {
    if (*task_list == NULL) { // if list is empty, new task becomes first task
        *task_list = new_task;
        return;
    }
    else { // not empty, traverse to the end of the list and pin 'new_task' to the end of the list
        Task* temp = *task_list;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_task;
    }
}

bool higher_priority(Task *taskA, Task *taskB){
    int priorityA = taskA->priority;
    int priorityB = taskB->priority;

    // for non preemptive priority, service time = remaining time before task starts
        // every task in the ready queue has not been run 
    int remainingTimeA = taskA->remaining_time;
    int remainingTimeB = taskB->remaining_time;

    int arrivalTimeA = taskA->arrival_time;
    int arrivalTimeB = taskB->arrival_time;

    // first check priority
    if (priorityA < priorityB)
        return true;
    if (priorityA > priorityB)
        return false;

    // then service/remaining time
    if (remainingTimeA < remainingTimeB)
        return true;
    if (remainingTimeA > remainingTimeB)
        return false;

    // revert to arrival time
    if (arrivalTimeA < arrivalTimeB)
        return true;
    else
        return false;
}

Task* pop_highest_priority(Task** ready_queue) {
    // best is a ptr to the 'next' ptr that references the task w/ the highest priority
    // currently the 'best task' is the first task in the ready queue
    Task** best = ready_queue;
    Task* first = *ready_queue; 

    // points to the 'next ptr' that references the second task
    Task** curr = &(first->next);
    while (*curr != NULL) {
        // task being walked is higher priority than current 'best'
        if (higher_priority(*curr, *best)) {
            best = curr;
        }
        // either way advance to the next node's 'next' ptr, 
        Task* temp = *curr;
        curr = &(temp->next);
    }

    // the winning task, ie the task the 'best' ptr points to
    Task* task = *best;

    // rewriting the 'next' ptr of the node so it skips over the 'winner'
    // and leads to whatever node is after it
    *best = task->next;

    // detach the winner
    task->next = NULL;
    return task;

}

void print_header() {
    // printing labels
    printf("%-10s %-10s %-10s %s\n", "time", "cpu", "priority", "ready queue (tid/rst)");
    printf("----------------------------------------------------\n");
}

void print_table(int time, Task* cpu, Task*rq) {
    if (cpu != NULL) { // if cpu not empty (busy) print task & info
        printf("%-10d %c%-9d %-10d", time, cpu->task_id, cpu->remaining_time, cpu->priority);
    }
    else { // if empty, print time and empty strings
        printf("%-10d %-10s %-10s", time, " ", " ");
    }

    if (rq == NULL) { // if ready queue empty
        printf("--");
    }
    else { // if contains task(s), print
        Task* curr = rq;
        while(curr != NULL) {
            printf("%c%d %d  ", curr->task_id, curr->remaining_time, curr->priority);
            curr = curr->next;
        }
    }
    printf("\n");
}

void print_tasks(Task* complete) {

    // printing header
    printf("\n\n%-10s %-10s %-10s %-10s %-10s %-10s %-10s\n",
    "   ", "    ", "arrival", "service", "completion", "response", "wait");

    printf("%-10s %-10s %-10s %-10s %-10s %-10s %-10s\n",
    "tid", "prio", "time", "time", "time", "time", "time");

    printf("-----------------------------------------------------------------------\n");

    // print completed tasks by ascending tid (alphabetical order)
    for (char tid = 'A'; tid <= 'Z'; tid++) { 
        // traverse completed task list for the current tid
        Task *curr = complete;
        while (curr != NULL) {
            if (curr->task_id == tid) { // tid match in list
                printf("%-10c %-10d %-10d %-10d %-10d %-10d %-10d\n", curr->task_id, 
                curr->priority, curr->arrival_time, curr->service_time, 
                curr->completion_time, curr->response_time, curr->wait_time);
            }
            // next task
            curr = curr->next;
        }
    }
}

void print_times(Task* complete) {
    // print header
    printf("\n\n%-10s %-10s\n", "service", "wait");
    printf("%-10s %-10s\n", "time", "time");
    printf("---------  -------\n");

    // track the largest service time of all now completed task
    int max = 0; 
    Task* curr = complete;
    while (curr != NULL) {
        if (curr->service_time > max) {
            max = curr->service_time;
        }
        curr = curr->next;
    }
    // check each possible service time in ascending order 1,2,3...
    for (int s = 1; s <= max; s++) { 
        Task* temp = complete;
        while (temp != NULL) {
            if (temp->service_time == s) {
                printf("%-10d %-10d\n", temp->service_time, temp->wait_time);
            }
            temp = temp->next;
        }
    }
    printf("\n");
}


// notArrivedYet is a pointer to the first task in the linked list
// ie points to the same Task that head in main points to
void priority_func(Task* notArrivedYet) { 
    printf("Priority scheduling results:\n");
    print_header();

    // list (ptrs) for tasks in different states
    Task* cpu = NULL;
    Task* rq = NULL;
    Task* complete = NULL;

    // ie 'discrete time value' / ticker
    int time = 0;
    // while there are still tasks to be seen in any of the three lists
    while (notArrivedYet != NULL || rq != NULL || cpu != NULL) {
        while (notArrivedYet != NULL && notArrivedYet->arrival_time == time) {
            // remove task from list and save it in a ptr...to be added to the rq
            Task* current_arrival = pop_task(&notArrivedYet);
            // add the task to the ready queue
            push_task(&rq, current_arrival);
        }
        if (cpu == NULL && rq != NULL) { 
            // remove the task w/ the highest prio. & assign it to cpu
            cpu = pop_highest_priority(&rq);
        }

        print_table(time, cpu, rq);

        if (cpu != NULL) { // tasks still on cpu
            // decrement time remaining for task 
            cpu->remaining_time--;
            if (cpu->remaining_time == 0) { // task has completed
                // time has not been incremed yet
                cpu->completion_time = time + 1;

                // add task to list of completed tasks
                push_task(&complete, cpu);

                // cpu is now empty ie ptr is NULL
                cpu = NULL;
            }
        }
        // increment time after all arrivals, scheduling, and completions have bee checked
        time++;
        
    }
    
    Task* curr = complete;
    while (curr != NULL) { // calculate resp. time & wait time for each task completed
        // resp. time = total time spent to finish, ie time arrived to time completed
        curr->response_time = curr->completion_time - curr->arrival_time;
        // wait time = of resp. time, time spent waiting in the ready queue
        curr->wait_time = curr->response_time - curr->service_time;
        curr = curr->next;
    }

    print_tasks(complete);
    print_times(complete);
    // free finished tasks
    Task *temp;
    while (complete != NULL) {
        temp = complete;
        complete = complete->next;
        free(temp);
    }
}

Task* find_highest_priority(Task* ready_queue) {
    // initally, the 'best' task is the first one
    // and start comparing from the second task
    Task* best = ready_queue;
    Task* curr = ready_queue->next;

    while (curr != NULL) {
        if (higher_priority(curr, best)) {
            best = curr;
        }
        curr = curr->next;
    }
    // return highest_priority
    return best;
}


void custom_func(Task* notArrivedYet) {
    printf("Custom (preemptive) scheduling results:\n");
    print_header();

    // list (ptrs) for tasks in different states
    Task *cpu = NULL;
    Task *rq = NULL;
    Task *complete = NULL;

    // ie 'discrete time value' / ticker
    int time = 0;
    // while there are still tasks to be seen in any of the three lists
    while (notArrivedYet != NULL || rq != NULL || cpu != NULL) {
        while (notArrivedYet != NULL && notArrivedYet->arrival_time == time) {
            // remove task from list and save it in a ptr...to be added to the rq
            Task *current_arrival = pop_task(&notArrivedYet);
            // add the task to the ready queue
            push_task(&rq, current_arrival);
        }
        if (cpu == NULL && rq != NULL) {
            // remove the task w/ the highest prio. & assign it to cpu
            cpu = pop_highest_priority(&rq);
        }
        // check for preemption if both cpu and ready queue contain tasks
        else if (cpu != NULL && rq != NULL) {
            Task* t = find_highest_priority(rq);

            // prempt the running task (on the cpu) if the waiting task is higher prio.
            if (higher_priority(t, cpu)) {
                // put task on cpu back on ready queue
                push_task(&rq, cpu);
                // take waiting task off ready queue onto cpu
                cpu = pop_highest_priority(&rq);
            }
        }

        print_table(time, cpu, rq);

        if (cpu != NULL) { // if there is a task on cpu
            // decrement time remaining for task
            cpu->remaining_time--;
            if (cpu->remaining_time == 0) { // task has completed
                // time has not been incremeted yet
                cpu->completion_time = time + 1;

                // add task to list of finished tasks
                push_task(&complete, cpu);

                // cpu is now empty
                cpu = NULL;
            }
            else { // if not done, lower priority (increment int)
                cpu->priority++;
            }
        }
        time++;
    }
    Task *curr = complete;
    while (curr != NULL) { // calculate resp. time & wait time for each task completed
        // resp. time = total time spent to finish, ie time arrived to time completed
        curr->response_time = curr->completion_time - curr->arrival_time;
        // wait time = of resp. time, time spent waiting in the ready queue
        curr->wait_time = curr->response_time - curr->service_time;
        curr = curr->next;
    }

    print_tasks(complete);
    print_times(complete);

    // free finished tasks
    Task *temp;
    while (complete != NULL) {
        temp = complete;
        complete = complete->next;
        free(temp);
    }
}

int main(int argc, char* argv[]) {
    // will store data read from input file
    int arrival;
    int service;
    int priority;

    char tid = 'Y'; 
    Task* head = NULL;
    // scan the input file for arrival, service, and priority values in that order
    while (scanf("%d %d %d", &arrival, &service, &priority) == 3) {
        // store data in new task struct
        Task* task = malloc(sizeof(Task));
        task->arrival_time = arrival;
        task->service_time = service;
        task->priority = priority;
        task->task_id = tid;
        task->next = NULL; // initialize 'next' ptr to NULL
        task->remaining_time = service;

        // alphabetic tids, roll over to A after Z
        if (tid == 'Z') {
            tid = 'A';
        }
        else { // keep incrementing until Z is hit
            tid++;
        }
        if (head == NULL) { // if list empty
            head = task;
        }
        else { // if list not empty, insert new task node at the end of the list ie tail
            Task* last = head;
            while(last->next != NULL) {
                last = last->next;
            }
            last->next = task;
        }
    }

    // policy is given via command line arg
    char* policy = argv[1];
    if (strcmp(policy, "-prio")==0) {
        priority_func(head);
    }
    else if (strcmp(policy, "-custom")==0) {
        custom_func(head);
    }
    
    return 0;
}