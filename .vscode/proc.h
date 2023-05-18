typedef struct Proc {
    int pid;
    int arrival_time;
    int burst_time;
    int memory_required;
    int time_left;
    int priority;
    int device_required;
} Proc;
