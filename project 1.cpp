/*
Chris DeStefano
CIS - 3207 Sec 003
Dr. Tamer Aldwairi 
9-17-2019
Project 1 
*/
#include <fstream>
#include <iostream> 
#include <string> 
#include <cstring>  
#include <queue> 
#include <thread> 
#include <chrono> 
#include <algorithm> 
#include <iomanip> 

using namespace std;

//determines the max number of jobs the system can generate 

    #define MAXJOBS 500

// struct to hold the constant values from the txt file 
struct scheduling_info
  {
    int seed;
    int init_time;  
    int FIN_TIME ;
    int ARRIVE_MIN;
    int ARRIVE_MAX; 
    int QUIT_PROB ;
    int CPU_MIN ;
    int CPU_MAX ;
    int DISK1_MIN; 
    int DISK1_MAX;
    int DISK2_MIN ;
    int DISK2_MAX ;
 }; 

//struct that defines a "job"
struct job 
{
    int ARRIVE_TIME; 
    string name; 
    bool need_io; 
    bool still_running; 
    int arr; //time job arrived  
    int fin; //time job finished 
};

// struct to give the priority que a priority 
struct Prio {
   
    bool operator() (job &a, job &b){

        return a.ARRIVE_TIME > b.ARRIVE_TIME; 
    }

};



//prototypes 
scheduling_info read_constants (string filename);
void print_struct (scheduling_info SI); 
priority_queue <job, vector <job>, Prio> random_Jobs (); 
int CPU_Handler (queue <job> &C, priority_queue <job, vector <job>, Prio> &P, scheduling_info SI2, int &t, int C_size[], int &C_counter, int P_size[], int &P_counter, int CPU_response[], int &CPU_res_counter, int &job_counter); 
int Disk1_Handler (queue <job> &D, priority_queue <job, vector <job>, Prio> &P, scheduling_info SI2, int &t, int D_size[], int &D_counter, int P_size[], int &P_counter, int D1_response[], int &D1_res_counter, int &job_counter); 
int Disk2_Handler (queue <job> &D2, priority_queue <job, vector <job>, Prio> &P, scheduling_info SI2, int &t,int D2_size[], int &D2_counter,int P_size[], int &P_counter, int D2_response[], int &D2_res_counter, int &job_counter ); 
void write_Job_Log (string job); 
void write_Stat_Log(int P_size[], int C_size[], int D_size[], int D2_size[], int P_counter, int C_counter, int D_counter, int D2_counter,  int time, int thru[], scheduling_info SI2, float Util[], int CPU_res[], int D1_res[], int D2_res[], int counter[], int t);
int find_max(int A[], int size); 
void initial_zero(int A[], int size); 



/* 
 Handles jobs in the CPU
*/
int CPU_Handler (queue <job> &C, priority_queue <job, vector <job>, Prio> &P, scheduling_info SI2, int &t, int C_size[], int &C_counter, int P_size[], int &P_counter, int CPU_response[], int &CPU_res_counter, int &job_counter){


C.push(P.top()); // pushes the first member in priority q CPU queue
C_size[C_counter] = C.size(); 
C_counter++; 
string job_arrives = P.top().name + " Arrives @ CPU"; 
string job_finishes =  P.top().name + " Finishes @ CPU"; 
string job_quits = P.top().name + " Returns to que from CPU"; 

P.pop();  // removes the top member of the prio q
P_size[P_counter] = P.size(); 
P_counter++; 

write_Job_Log(job_arrives + " at " + to_string(t)); // prints the log    
C.front().arr = t; 


        

int job_time = (rand () % (SI2.CPU_MAX - SI2.CPU_MIN) + SI2.CPU_MIN); //generates a random job time based on the constant file 
this_thread::sleep_for(chrono::milliseconds(job_time)); // simulates the CPU running a job for the value of job_time
C.front().ARRIVE_TIME += job_time; 
t += job_time; 

//cout << C.front().ARRIVE_TIME << endl; 

int stay = (rand ()% 100) +1; //determines the chance that job stays int he CPU instead of quiting or finishing because  

 int quit = (rand() % 100) +1; /// determines quit_prob for disk IO 

 while(C.front().still_running == true){ // if the job being worked on (first job in que) is not the newest to arrive 
        int finish = (rand() % 100) +1; // then the current job has a 75% chance to finish 
            if (finish <= 75){
                string running_job_finishes = C.front().name + " Finishes @ CPU"; 
                write_Job_Log(running_job_finishes + " at " + to_string(t)); 
                C.front().still_running = false; 
                C.front().fin = t; 
                CPU_response[CPU_res_counter] = C.front().fin - C.front().arr; 
                CPU_res_counter ++ ; 
                C.pop(); 
                job_counter++;
                C_size[C_counter] = C.size(); 
                C_counter++; 
            }
    }


// Handles the job coming into the CPU 
     if (stay <= 50){
        C.front().still_running = true; 
        return job_time; 
     }
     else if (quit >= SI2.QUIT_PROB){
        C.front().need_io = true; 
        P.push(C.front());
        P_size[P_counter] = P.size(); 
        P_counter++; 
        C.front().fin = t; 
        CPU_response[CPU_res_counter] = C.front().fin - C.front().arr; 
        CPU_res_counter ++ ; 
        C.pop(); 
        job_counter++; 
        C_size[C_counter] = C.size(); 
        C_counter++; 
        write_Job_Log(job_quits+ " at " + to_string(t));
    }
    else {
        C.front().fin = t; 
        CPU_response[CPU_res_counter] = C.front().fin - C.front().arr; 
        CPU_res_counter ++ ; 
        C.pop();
        job_counter++; 
        C_size[C_counter] = C.size(); 
        C_counter++; 
        write_Job_Log(job_finishes+ " at " + to_string(t));
    }
     


  // prints the log    

    return job_time; 
}


/*

Handles Jobs in the disc 1

*/
int Disk1_Handler (queue <job> &D, priority_queue <job, vector <job>, Prio> &P, scheduling_info SI2, int &t, int D_size[], int &D_counter, int P_size[], int &P_counter, int D1_response[], int &D1_res_counter, int &job_counter){

D.push(P.top()); // pushes the first member in priority q CPU queue
D_size[D_counter] = D.size();
D_counter ++; 
string job_arrives = P.top().name + " Arrives @ Disk 1"; 
string job_finishes =  P.top().name+ " Finishes @ Disk 1"; 
string job_quits = P.top().name + " Returns to que from Disk 1";
P.pop();  // removes the top member of the prio q 
P_size[P_counter] = P.size(); 
P_counter++; 

 
write_Job_Log(job_arrives+ " at " + to_string(t)); // prints the log    
D.front().arr = t; 
        

int job_time = (rand () % (SI2.DISK1_MAX - SI2.DISK1_MIN) + SI2.DISK1_MIN); //generates a random job time based on the constant file 
this_thread::sleep_for(chrono::milliseconds(job_time)); // simulates the CPU running a job for the value of job_time
t += job_time; 
  
 
int stay = (rand ()% 100) +1; // way to simulate jobs being added to other compents while compents are working on current jobs 

    while(D.front().still_running == true){ // if the job being worked on (first job in que) is not the newest to arrive 
        int finish = (rand() % 100) +1; // then the current job has a 75% chance to finish 
            if (finish <= 75){
                D.front().still_running = false; 
                string running_job_finishes = D.front().name + " Finishes @ Disk 1"; 
                write_Job_Log(running_job_finishes+ " at " + to_string(t)); 
                D.front().fin = t; 
                D1_response[D1_res_counter] = D.front().fin - D.front().arr; 
                D1_res_counter ++ ; 
                D.pop(); 
                job_counter ++;
                D_size[D_counter] = D.size();
                D_counter ++; 
            }
    }

    
    if (stay <= 50){
        D.front().still_running = true; 
        return job_time; 
     } 
    else {
         D.front().fin = t; 
         D1_response[D1_res_counter] = D.front().fin - D.front().arr; 
         D1_res_counter ++ ; 
         D.pop(); 
        job_counter++;
         D_size[D_counter] = D.size();
         D_counter ++; 
         write_Job_Log(job_finishes+ " at " + to_string(t));     
    }



    return job_time; 
}

/*

Handles Jobs in the disc 2

*/
int Disk2_Handler (queue <job> &D2, priority_queue <job, vector <job>, Prio> &P, scheduling_info SI2, int &t , int D2_size[], int &D2_counter, int P_size[], int &P_counter,int D2_response[], int &D2_res_counter, int &job_counter ){

D2.push(P.top()); // pushes the first member in priority q CPU queue
D2_size[D2_counter] = D2.size();
D2_counter ++; 

string job_arrives = P.top().name + " Arrives @ Disk 2"  ; // try concatinating with a sepret string in the log file? 
string job_finishes =  P.top().name + " Finishes @ Disk 2"; 
string job_quits = P.top().name + " Returns to que from Disk 2"; 

P.pop();  // removes the top member of the prio q 
P_size[P_counter] = P.size(); 
P_counter++; 

write_Job_Log(job_arrives + " at " + to_string(t) ); // prints the log    
D2.front().arr = t; 
        

int job_time = (rand () % (SI2.DISK2_MAX - SI2.DISK2_MIN) + SI2.DISK2_MIN); //generates a random job time based on the constant file 
this_thread::sleep_for(chrono::milliseconds(job_time)); // simulates the CPU running a job for the value of job_time
t+= job_time; 
  
 int stay = (rand ()% 100) +1; //determines the chance that a job will not be finished after one round of processing 

    while(D2.front().still_running == true){ // if the job being worked on (first job in que) is not the newest to arrive 
            int finish = (rand() % 100) +1; // then the current job has a 75% chance to finish 
                if (finish <= 75){
                    string running_job_finishes = D2.front().name + " Finishes @ Disk 2"; 
                    write_Job_Log(running_job_finishes+ " at " + to_string(t)); 
                    D2.front().still_running = false; 
                    D2.front().fin = t; 
                    D2_response[D2_res_counter] = D2.front().fin - D2.front().arr; 
                    D2_res_counter ++ ; 
                    D2.pop(); 
                    job_counter++;
                    D2_size[D2_counter] = D2.size();
                    D2_counter ++; 
                }
        }


    if (stay <= 50){
        D2.front().still_running = true; 
        return job_time; 
     }
     
    else {
        D2.front().fin = t; 
        D2_response[D2_res_counter] = D2.front().fin - D2.front().arr; 
        D2_res_counter ++ ;
        job_counter++; 
        D2.pop();
        D2_size[D2_counter] = D2.size();
        D2_counter ++;  
        write_Job_Log(job_finishes+ " at " + to_string(t));     

    }


    return job_time; 
}






/* 
print struct values 
*/
void print_struct (scheduling_info SI){

    cout << SI.seed<< endl;  
    cout << SI.init_time<< endl; 
    cout << SI.FIN_TIME<< endl; 
    cout << SI.ARRIVE_MIN<< endl; 
    cout << SI.ARRIVE_MAX<< endl; 
    cout << SI.CPU_MIN << endl; 
    cout << SI.QUIT_PROB<< endl; 
    cout << SI.CPU_MAX<< endl; 
    cout << SI.DISK1_MAX<< endl; 
    cout << SI.DISK1_MIN<< endl; 
    cout << SI.DISK2_MAX<< endl; 
    cout << SI.DISK2_MIN<< endl; 


}
/*
Generates a random number of jobs and inserts them into a priority queue 
*/
priority_queue <job, vector <job>, Prio> random_Jobs (){


int random_Num= (rand () % MAXJOBS) + 1; //determines the number of jobs to be run (randomly)
priority_queue <job, vector <job>, Prio> prio_que; 
 
     
     for (int i =0; i <= random_Num; i++){
         job temp; 
         temp.name = "job " + to_string(i);
        temp.ARRIVE_TIME = (rand() % 20 )+1; //termines the arrival time of the jobs (the priority)
        temp.need_io = false; 
        temp.still_running = false; 
        int arr =0; 
        int fin =0; 
        prio_que.push(temp); 


     }



return prio_que; 
}

/*
Reads the constant values from the text file
*/
scheduling_info read_constants (string filename){

// ifstream - input
// ofstream - output 
ifstream File; 
File.open("Constants.txt"); 

    if (!File){
        cout << "Can't open file" << endl; 
        exit (1); 
    }

scheduling_info SI; 
string name; 


    File >> name >> SI.seed;    //name is stored in the temp variable name and the value is store in the SI.val
    File >> name >> SI.init_time;
    File >> name >> SI.FIN_TIME;
    File >> name >> SI.ARRIVE_MIN;
    File >> name >> SI.ARRIVE_MAX;
    File >> name >> SI.QUIT_PROB;
    File >> name >> SI.CPU_MIN ; 
    File >> name >> SI.CPU_MAX;
    File >> name >> SI.DISK1_MIN;
    File >> name >> SI.DISK1_MAX;
    File >> name >> SI.DISK2_MIN;
    File >> name >> SI.DISK2_MAX;

    File.close(); 
    return SI; 
}


int find_max(int A[], int size){
int max =A[0]; 
    for (int i = 0; i <= size; i++){
        if (max < A[i])
            max = A[i]; 
    }
    return max; 
}

void write_Job_Log (string job){
ofstream File; 
File.open("Log.txt", std::ofstream::out | std::ofstream::app); 

    if (!File){
        cout << "Can't open file" << endl; 
        exit (1); 
    }

File << job << "\n"; 
File.close(); 
}

void write_Stat_Log(int P_size[], int C_size[], int D_size[], int D2_size[], int P_counter, int C_counter, int D_counter, int D2_counter,  int time, int thru[], scheduling_info SI2, float Util[], int CPU_res[], int D1_res[], int D2_res[], int counter[], int t){

ofstream File; 
File.open("Statistics Log.txt"); 

    if (!File){
        cout << "Can't open file" << endl; 
        exit (1); 
    }

int P_max =0; 
int C_max =0;
int D_max =0; 
int D2_max =0; 
int P_average = 0; 
int C_average = 0; 
int D_average = 0; 
int D2_average =0; 

P_max = find_max(P_size, P_counter); 
C_max = find_max(C_size, C_counter); 
D_max = find_max(D_size, D_counter); 
D2_max = find_max(D2_size, D2_counter); 

int sum  = 0; 

for (int i = 0; i <= P_counter; i++){
    sum += P_size[i]; 
}
P_average = sum/P_counter; 

sum =0; 

for (int i = 0; i <= C_counter; i++){
    sum += C_size[i]; 
}
C_average = sum/C_counter; 

sum =0; 

for (int i = 0; i <= D_counter; i++){
    sum += D_size[i]; 
}
D_average = sum/D_counter; 

sum =0; 

for (int i = 0; i <= D2_counter; i++){
    sum += D2_size[i]; 
}

D2_average = sum/D2_counter; 
sum =0; 

int CPU_response_max =0; 
int D1_response_max =0; 
int D2_response_max =0;
int CPU_response_avg =0;  
int D1_response_avg =0; 
int D2_response_avg =0;

CPU_response_max = find_max(CPU_res, t); 
D1_response_max = find_max(D1_res, t); 
D2_response_max = find_max(D2_res, t);

sum=0; 
for (int i = 0; i <= counter[0]; i++){ // counter[0] is the number of times that CPU_res was  incrimented 
    sum += CPU_res[i]; 
}
CPU_response_avg = sum/counter[0]; 

sum =0; 
for (int i = 0; i <= counter[1]; i++){
    sum += D1_res[i]; 
}
D1_response_avg = sum/counter[1];

sum=0; 
for (int i = 0; i <= counter[2]; i++){
    sum += D2_res[i]; 
}
D2_response_avg= sum/counter[2];


float CPU_thru =0; 
float  D1_thru =0; 
float D2_thru =0; 

CPU_thru = ((float)thru[0]/(float)time); 
D1_thru = ((float)thru[1]/(float)time); 
D2_thru = ((float)thru[2]/(float)time); 



File << "STATISTICS:\n\n"; 
File << "AVERAGE AND MAX VALUES OF EACH QUEUE: \nPRIORITY QUEUE:\nMax: " << P_max << "\nAverage: " << P_average << "\n\n";   
File << "CPU QUEUE:\nMax: " << C_max << "\nAverage: " << C_average << "\n\n";
File << "DISK 1 QUEUE:\nMax: " << D_max << "\nAverage: " << D_average << "\n\n";
File << "DISK 2 QUEUE:\nMax: " << D2_max << "\nAverage: " << D2_average << "\n\n";
File <<"UTILIZATION OF EACH COMPONENT:\nCPU: " << Util[0] << "\nDISK1: " << Util[1] << "\nDISK2: " << Util[2] << "\n"; 
File <<"\nRESPONSE TIME OF EACH COMPONENT:\nCPU:\nMax: " <<CPU_response_max<< "\nAverage: " <<CPU_response_avg; 
File <<"\n\nDisk 1:\nMax: " <<D1_response_max<< "\nAverage: " <<D2_response_avg; 
File <<"\n\nDisk 2:\nMax: " <<D2_response_max<< "\nAverage: " <<D2_response_avg; 
File <<"\n\nTHROUGHPUT OF EACH COMPONENT:\nCPU: " << CPU_thru << "\nDisk 1: " << D1_thru << "\nDisk 2: " << D2_thru;











File.close(); 

}


void initial_zero(int A[], int size){
    for (int i = 0; i <= size; i++){
        A[i] =0; 
    }
}



/*
Main 
*/
int main (){

ofstream File; 
File.open("Log.txt"); 

    if (!File){
        cout << "Can't open file" << endl; 
        exit (1); 
    }



scheduling_info SI2; 
SI2 = read_constants("Constants.txt"); 
    File << " CONSTANTS: \n"; 
    File << "SEED " << SI2.seed << "\n";    //name is stored in the temp variable name and the value is store in the SI.val
    File << "INIT_TIME " << SI2.init_time<< "\n";
    File << "FIN_TIME " << SI2.FIN_TIME<< "\n";
    File << "ARRIVE_MIN " << SI2.ARRIVE_MIN<< "\n";
    File << "ARRIVE_MAX " << SI2.ARRIVE_MAX<< "\n";
    File << "QUIT_PROB " << SI2.QUIT_PROB<< "\n";
    File << "CPU_MIN " << SI2.CPU_MIN << "\n";
    File << "CPU_MAX " << SI2.CPU_MAX<< "\n";
    File << "DISK1_MAX " << SI2.DISK1_MAX<< "\n";
    File << "DISK1_MIN " << SI2.DISK1_MIN<< "\n";
    File << "DISK2_MAX " << SI2.DISK2_MAX<< "\n";
    File << "DISK2_MIN " << SI2.DISK2_MIN<< "\n";

 File.close();

srand(SI2.seed); 

queue <job> CPU;     
queue <job> Disk_1; 
queue <job> Disk_2; 
priority_queue <job, vector <job>, Prio> Prio_que;

int arr_size = MAXJOBS * 5; 
// Stastics variables 
int Prio_size[arr_size];
int CPU_total[arr_size]; 
int Disk1_total[arr_size];
int Disk2_total[arr_size]; 

//initialize all the values to zero so it doesent mess up data

initial_zero(Prio_size, arr_size);
initial_zero(CPU_total, arr_size);
initial_zero(Disk1_total, arr_size);
initial_zero(Disk2_total, arr_size);


int CPU_counter =0; 
int Disk1_counter =0; 
int Disk2_counter =0; 
int Prio_counter =1;

int job_time_CPU =0; 
int job_time_D1 =0;
int job_time_D2 =0;  

float Util[3]; 
Util[0] = 0.0; 
Util[1] = 0.0; 
Util[2] = 0.0; 


Prio_que = random_Jobs();   
//cout << "hello" << endl; 
Prio_size[0] = Prio_que.size(); // entry one in the size array is always the total size of Priority que. 

int time = SI2.init_time;

int t = Prio_que.size(); 

int CPU_response[t]; 
int D1_response[t]; 
int D2_response[t]; 
initial_zero(CPU_response, t); 
initial_zero(D1_response, t); 
initial_zero(D2_response, t); 


//counter is the number of jobs processed 
int CPU_res_counter =0; 
int D1_res_counter =0;
int D2_res_counter =0;

int job_counter_CPU =0; 
int job_counter_D1 =0; 
int job_counter_D2 =0; 
int counter [3]; 
initial_zero(counter, 3); 




write_Job_Log("\n TOTAL JOBS TO BE PROCESSED: \n" + to_string(Prio_que.size())); 

write_Job_Log("\n EVENTS:");

/// main loop to run the simulator 
while (Prio_que.size() > 0 && time <= SI2.FIN_TIME /* && CPU.size() >= 0 && Disk_1.size() >= 0 && Disk_2.size() > 0*/){ //while the que has items and the and the run time does not equal fin_time
    //CPU.size() <= Disk_1.size() && CPU.size() <= Disk_2.size() && CPU.size() < SI2.CPU_MAX 
   if (CPU.size() <= Disk_1.size() && CPU.size() <= Disk_2.size() && Prio_que.top().need_io == false){
      job_time_CPU += CPU_Handler(CPU, Prio_que, SI2, time, CPU_total, CPU_counter, Prio_size, Prio_counter, CPU_response, CPU_res_counter, job_counter_CPU ); // when a job goes to the CPU que 
      
    }
    else if (Disk_1.size() <= CPU.size() && Disk_1.size() <= Disk_2.size()) {
       job_time_D1 += Disk1_Handler (Disk_1, Prio_que, SI2, time, Disk1_total, Disk1_counter, Prio_size, Prio_counter,D1_response, D1_res_counter, job_counter_D1); 

    }
    else if (Disk_2.size() <=    CPU.size() && Disk_2.size() <= Disk_1.size()) {
        job_time_D2 += Disk2_Handler (Disk_2, Prio_que, SI2, time, Disk2_total, Disk2_counter, Prio_size, Prio_counter, D2_response, D2_res_counter, job_counter_D2);
    }
    else if (CPU.size() == Disk_1.size() && CPU.size() == Disk_2.size() && Prio_que.top().need_io == false){ //if all que sizes are equal and the job can be processed anywhere 
        int d = (rand() % 3) +1; 

            if (d == 1){
               job_time_CPU += CPU_Handler(CPU, Prio_que, SI2, time, CPU_total, CPU_counter, Prio_size, Prio_counter, CPU_response, CPU_res_counter, job_counter_CPU );
            }
            if (d == 2){
                job_time_D1 +=  Disk1_Handler (Disk_1, Prio_que, SI2, time, Disk1_total, Disk1_counter, Prio_size, Prio_counter, D1_response, D1_res_counter, job_counter_D1);
            }
            if (d==3) {
               job_time_D2 += Disk2_Handler (Disk_2, Prio_que, SI2, time, Disk2_total, Disk2_counter, Prio_size, Prio_counter, D2_response, D2_res_counter, job_counter_D2);
            }
    }
    else if (Prio_que.top().need_io == true && Disk_1.size() == Disk_2.size()){ // if the job needs disk io and all sizes are equal 
         int d = (rand() % 2) +1; 
            if (d == 1){
              job_time_D1 += Disk1_Handler (Disk_1, Prio_que, SI2, time, Disk1_total, Disk1_counter, Prio_size, Prio_counter, D1_response, D1_res_counter, job_counter_D1);
            }
            if (d == 2) {
              job_time_D2 +=  Disk2_Handler (Disk_2, Prio_que, SI2, time, Disk2_total, Disk2_counter, Prio_size, Prio_counter, D2_response, D2_res_counter, job_counter_D2 );
            }
    }



}

counter[0] = CPU_res_counter; 
counter[1] = D1_res_counter; 
counter[2] = D2_res_counter; 



float temp1, temp2, temp3 = 0.0; 
temp1 = ((float)job_time_CPU/(float)time);
temp2 = ((float)job_time_D1/(float)time);
temp3 = ((float)job_time_D2/(float)time);
Util[0] = temp1; 
Util[1] = temp2;
Util[2] = temp3;  
int job_counter[3]; 
initial_zero(job_counter, 3); 
job_counter[0] = job_counter_CPU; 
job_counter[1] = job_counter_D1; 
job_counter[2] = job_counter_D2; 



write_Job_Log("\n RUNETIME: \nTotal run time = " + to_string(time) + " milliseconds\n"); 
write_Stat_Log(Prio_size, CPU_total, Disk1_total, Disk2_total, Prio_counter, CPU_counter, Disk1_counter, Disk2_counter, time, job_counter, SI2, Util, CPU_response, D1_response, D2_response, counter, t); 







return 0; 
}


