#include <stdio.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <utmp.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/utsname.h>
#include <getopt.h>
#include <math.h>



void display_Top_Info();
void store_memArr();
int display_Third_Info();
void display_Last_Info();
void display_cores();
void display_time_Last_Info();
double calculate_virtual_used_gb();
double calculate_cpu_usage();
void store_cpuArr();
void fcn_for_print_memArr();
void store_newline_memArr();
void reserve_space();
void memory_graphics();
void reserve_space();

//void cpu_graphics();
//void line_up();
void cpu_graphics(int sampleIndex, char cpuArr[][200], float curCpuUsage, int sequential) {
    int baseBarCount = 3; // 기본 바 개수
    int additionalBars = (int)curCpuUsage; // CPU 사용량에 따른 추가 바 개수

    // CPU 사용 정보 문자열 초기화
    char cpuUsageStr[200] = "         "; // 시작 공백
    if (i == 0) {
        *default_num = 3; // Assuming 3 as a default_num starting value for the number of bars
        sprintf(cpuArr[i], "|||%.2f", cur_cpuUsage);
    } 
    for (int i = 0; i < baseBarCount + additionalBars; ++i) {
        strcat(cpuUsageStr, "|");
    }

    char usagePercent[50];
    sprintf(usagePercent, " %.2f%%", curCpuUsage);
    strcat(cpuUsageStr, usagePercent);

    strcpy(cpuArr[sampleIndex], cpuUsageStr);

    // sequential 모드에 따라 출력 방식 조정
    if(sequential) {
        for (int j = 0; j <= sampleIndex; j++) {
            printf("%s\n", cpuArr[j]);
        }
    } else {
        printf("%s\n", cpuArr[sampleIndex]);
    }
}


// void cpu_graphics(int sequential, char cpuArr[][200], int *default_num, float cur_cpuUsage, float *prev, int i) {
//     int diff_bar = 0;
//     char cpuStr[200] = "\0";
//     if (i == 0) {
//         *default_num = 3; // Assuming 3 as a default_num starting value for the number of bars
//         sprintf(cpuArr[i], "         |||%.2f", cur_cpuUsage);
//     } 
//     else {
//         diff_bar = (int)cur_cpuUsage - (int)(*prev);
//         *default_num += diff_bar;
//         strcpy(cpuArr[i], "         ");
//         for (int m = 0; m < *default_num; m++) {
//             strcat(cpuArr[i], "|");
//         }
//         sprintf(cpuStr, "%.2f", cur_cpuUsage);
//         strcat(cpuArr[i], cpuStr);
//     }

//     if(sequential) {
//         for (int j = 0; j <= i; j++) {
//             if (j == i)
//                 printf("%s\n", cpuArr[j]);
//             else
//                 printf("\n");
//         }
//     } 
//     else {
//         for (int h = 0; h <= i; h++) {
//             printf("%s\n", cpuArr[h]);
//         }
//     }

//     *prev = cur_cpuUsage;
// }

int main(int argc,char *argv[]){
    //  struct utmp utmp_user=malloc(sizeof(struct utmp));
    //  free(utmp_user);
    int samples = 10,tdelay = 1;
    int user=0,system=0,sequential=0,graphics=0;
    struct option long_options[] = {
        {"system", no_argument, 0, 's'}, 
        {"user", no_argument, 0, 'u'}, 
        {"graphics", no_argument, 0, 'g'},        
        {"sequential", no_argument, 0, 'a'}, 
        {"samples", optional_argument, 0, 'b'}, // both samples and tdelay may change after. so we use optinal_argument. 
        {"tdelay", optional_argument, 0, 'c'},
        {0,0,0,0}
    };
    int opt;
    while ((opt = getopt_long(argc, argv, "sugab::c::", long_options, NULL)) != -1){
        //so samples or -a maybe has arg or not so we use :: to tell.
        switch (opt) {
            case 's':
                system =1;
                break;
            case 'u':
                user = 1;
                break;
            case 'g':
                graphics =1;
                break;
            case 'a':
                sequential = 1;
                break;
            case 'b':
                if(optarg)samples = atoi(optarg); //so this line will store /update the value of samples.
            case 'c':
                if(optarg)tdelay = atoi(optarg);
        }
    }

    // if (argc > 1) {
    //     samples = atoi(argv[1]);
    // }

    // if (argc > 2) {
    //     tdelay = atoi(argv[2]);
    // }
    for(int ind = optind, i=0; ind < argc; ind++, i++) { // optind is global var.현재 처리 중인 인자의 위치(인덱스)현재 처리 중인 인자의 위치(인덱스)
        switch(i){
            case 0: 
                samples = atoi(argv[ind]); 
                break;
            case 1:
                tdelay = atoi(argv[ind]); 
                break;
        }
    }
    unsigned long prev_cpu[7];
    unsigned long curr_cpu[7];
    double virtual_used_gb=0.0;
    double  prev_used_gb=0.0;
    float cur_cpuUsage=0.00,prev_cpuUsage = 0.00;
    char memArr[samples][1024];
    char cpuArr[samples][200];
    
    int i =0;
    int default_num = 3;
    int CPU_GRAPH_START_LINE;
    int MEM_GRAPH_START_LINE;
    int userLine_count;
    // // char cpu_arr[samples][200];
    // if(user && !system &&!sequential)
    // {
    //     display_Top_Info(samples,tdelay,sequential,i);
    //     printf("-------------------------\n");
    //     display_Third_Info();
    //     printf("-------------------------\n");
    //     display_Last_Info();
    //     printf("-------------------------\n");
    //     exit(1);   

    // }    

    
        
    if(sequential){   //--seuential and/or --user --system etc.. 
        for (i = 0; i < samples; i++) { 
            store_cpuArr(prev_cpu); 
            sleep(tdelay); 
            display_Top_Info(sequential, samples, tdelay,i); 
            if(!user || (user && system)){ 
                printf("---------------------------------------\n");
                store_memArr(memArr,i); 
                virtual_used_gb = calculate_virtual_used_gb();
                if(graphics) 
                    memory_graphics(virtual_used_gb,&prev_used_gb,memArr,i); 
                
                fcn_for_print_memArr(sequential,samples,memArr,i); 
                if((user && system)||!system){ 
                    printf("---------------------------------------\n");
                    display_Third_Info(); 
                    printf("---------------------------------------\n");
                }

                display_cores();
                
                store_cpuArr(curr_cpu); 

                cur_cpuUsage = calculate_cpu_usage(prev_cpu, curr_cpu); 
                printf(" total cpu use: %.2f%%\n", cur_cpuUsage); 

                if(graphics)
                    cpu_graphics(sequential,cpuArr,&default_num,cur_cpuUsage,&prev_cpuUsage,i); 
            }else{ 
                printf("---------------------------------------\n");
                display_Third_Info();
                printf("---------------------------------------\n");
            }
            }
        }
        else{
            for( i = 0; i < samples; i++) {
                store_cpuArr(prev_cpu);
                sleep(tdelay);
                display_Top_Info(samples, tdelay,sequential,i);
                if((system &&  !user) &&!(sequential &&system)){ // no user in arg then we have to care about space of printing. 
                //--system. and/or --graphics, but no user. yet and/or --sequential.
                    store_memArr(memArr,i);
                    //printf("no stop");
                    //printf("should not running here");
                    virtual_used_gb = calculate_virtual_used_gb();
                    if(graphics){
                        memory_graphics(virtual_used_gb,&prev_used_gb,memArr,i);
                    }
                    //fcn_for_print_memArr(sequential,samples,memArr,i);
                    printf("------------------------------------------------\n");  
                    reserve_space(samples);
                    printf("----------------------------------------------------\n"); 
                    display_cores();
                    store_cpuArr(curr_cpu);
                    cur_cpuUsage = calculate_cpu_usage(prev_cpu,curr_cpu);
                    printf("total cpu use:%.2f%%\n",cur_cpuUsage );
                    

                    for (int j=0;j<samples+4;j++){
                        printf("\033[1A"); //move cursor up one line.
                    } 
                    fcn_for_print_memArr(sequential,samples,memArr,i);
                    
                    if(graphics && system){

                        printf("\033[%d;1H", CPU_GRAPH_START_LINE=18);
                        cpu_graphics(sequential,cpuArr,&default_num,cur_cpuUsage,&prev_cpuUsage,i); //if graphics option is given, display cpu graphics
                        
                    
                    }else{
                        printf("\033[3B"); //move cursor three line down.

                    }
                    

                    }
                    else if((user &&system)||!(user && system)&& !sequential){ //display w --user --system and/or --graphics, and/or sequential.
                        store_memArr(memArr,i);
                        //printf("runnign here\n");
                        virtual_used_gb = calculate_virtual_used_gb();
                        if(graphics){
                            memory_graphics(virtual_used_gb,&prev_used_gb,memArr,i);
                        }
                        //fcn_for_print_memArr(sequential,samples,memArr,i);
                        printf("------------------------------------------------\n");  
                        reserve_space(samples);
                        printf("----------------------------------------------------\n"); 
                        //printf("running here");
                        userLine_count= display_Third_Info();
                        printf("---------------------------------------\n");
                        display_cores();
                        store_cpuArr(curr_cpu);
                        cur_cpuUsage= calculate_cpu_usage(prev_cpu,curr_cpu);
                        printf("total cpu use:%.2f%%\n",cur_cpuUsage );

                        printf("\033[%d;1H", MEM_GRAPH_START_LINE=3); //,may chage after .
                        fcn_for_print_memArr(sequential,samples,memArr,i);
                        if(graphics){
                            CPU_GRAPH_START_LINE = userLine_count+20;
                            
                            //printf("userline is:%d\n",userLine_count);
                            printf("\033[%d;1H", CPU_GRAPH_START_LINE); //maybe
                            cpu_graphics(sequential,cpuArr,&default_num,cur_cpuUsage,&prev_cpuUsage,i);
                        }else{
                            for(int j=0; j<userLine_count+6; j++){
                                printf("\033[1B");
                            }
                            
                        }

                    }
                }
    }

    printf("------------------------------------\n");
    display_Last_Info();
    printf("----------------------------------\n");
    return 0;
    

    
    
}

void reserve_space(int samples){
    
    for(int i=0;i<samples+1; i++){
        printf("\n");
    }
}
double calculate_virtual_used_gb(){
    struct sysinfo sys_info;

    sysinfo(&sys_info); //get the information of system.(such as memory usage)
    double phys_total_gb = (float)sys_info.totalram/1024/1024/1024; 
    double phys_free_gb = (float)sys_info.freeram/1024/1024/1024;
    double phys_used_gb = (float)phys_total_gb-phys_free_gb;
    double swap_total_gb = (float)sys_info.totalswap/1024/1024/1024;
    double swap_free_gb = (float)sys_info.freeswap/1024/1024/1024;

    double virtual_used_gb = phys_used_gb+(swap_total_gb-swap_free_gb);
    
    return virtual_used_gb; 
}


   
void display_Top_Info(int samples, int tdelay,int sequential,int i) 
{
    /*Print the Nbr of samples: 10 --  every 1 secs.
     and Memory usage:4092 kilobytes. this is just a example of Top info.
    */
    
    
    //for memory usage:
    struct rusage usage_info; // from <sys/resource.h>, to use mAgetrusage();
    
    int result = getrusage(RUSAGE_SELF,&usage_info);
    if(sequential){
        printf(">>> iteration %d\n",i);
    }
    else{
        printf("\033[H\033[2J");
        printf("Nbr of samples: %d-- every %d secs\n", samples,tdelay);
    }
    if(result ==0){
        printf("Memory usage: %ld kilobytes\n", usage_info.ru_maxrss);
    }
   else{
    printf("fail to get Resource usage info");
   }
}
void store_memArr(char arr[][1024],int i){
    /*display/print Memo*/
    //아마도 array에 저장해 sequentially 다룰 필요.
    struct sysinfo sys_info;

    sysinfo(&sys_info); //get the information of system.(such as memory usage)
    double phys_total_gb = (float)sys_info.totalram/1024/1024/1024; 
    double phys_free_gb = (float)sys_info.freeram/1024/1024/1024;
    double phys_used_gb = (float)phys_total_gb-phys_free_gb;
    double swap_total_gb = (float)sys_info.totalswap/1024/1024/1024;
    double swap_free_gb = (float)sys_info.freeswap/1024/1024/1024;

    double virtual_used_gb = phys_used_gb+(swap_total_gb-swap_free_gb);
    double virtual_total_gb =(phys_total_gb+swap_total_gb);
    sprintf(arr[i],"%.2f GB / %.2f GB  -- %.2f GB / %.2f GB",
    phys_used_gb,phys_total_gb,virtual_used_gb,virtual_total_gb);
    

}


void fcn_for_print_memArr(int sequential,int samples,char memArr[][1024],int i){
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
    if(sequential){
        for(int k=0; k<samples; k++){
            if(k==i){
                
                printf("%s\n",memArr[k]);
                
            }
            else{
                printf("\n");
            }
        }
    }
    else{
        for(int j=0; j<=i; j++){
            printf("%s\n",memArr[j]);
        }
        
    }
}

void memory_graphics(double virtual_used_gb, double *prev_used_gb, char memArr[][1024], int i) {
    char graphicsStr[1024] = " |";
    double change = virtual_used_gb - *prev_used_gb;
    // 변화량을 백분율로 변환하고 반올림
    int symbolsCount = round(fabs(change) * 100);

    if (change >= 0.00 && change < 0.01) {
        strcat(graphicsStr, "o ");
    } else if (change < 0 && change > -0.01) {
        strcat(graphicsStr, "@ ");
    } else {
        for (int k = 0; k < symbolsCount; ++k) {
            strcat(graphicsStr, change < 0 ? ":" : "# ");
        }
        strcat(graphicsStr, change < 0 ? "@" : "* ");
    }

    char usageInfo[50];
    sprintf(usageInfo, "%.2f (%.2f)", change, virtual_used_gb);
    strcat(graphicsStr, usageInfo);

    strcpy(memArr[i], graphicsStr);
    *prev_used_gb = virtual_used_gb;

    for (int j = 0; j <= i; ++j) {
        printf("%s\n", memArr[j]);
    }
}

// void memory_graphics(double virtual_used_gb, double *prev_used_gb, char memArr[][1024],int i){
//     char graphicsArr[1024]="\0", diff_virtArr[1024]="\0";
//     double difference=0.00;
//     int j =0;
//     strcpy(graphicsArr," |");
//     if(i==0){
//         difference = 0.00;
//     }

//     else{
//         difference = virtual_used_gb-*(prev_used_gb);
//     }
//     if(difference>=0.00 && difference <0.01){ //if difference>=0
//         strcat(graphicsArr,"o "); //usually, or always(assumeption) start w o.
//     }
//     else if(difference <0 && difference >-0.01){ //<0
//         strcat(graphicsArr,"@ ");
        
//     }
//     else{
//         j = fabs((int)((difference-(int)difference+0.005)*100));
//         if(difference<0){
//             for(int v =0; v<j; v++){
//                 strcat(graphicsArr,":");
//             }strcat(graphicsArr,"@");

//         }
//         else{ //>=0
//             for(int k=0; k<j; k++){
//                 strcat(graphicsArr,"# ");
//             }strcat(graphicsArr,"* " );
//         }
//     }
//     *(prev_used_gb)=virtual_used_gb;
//     sprintf(diff_virtArr,"%.2f (%.2f)",difference,virtual_used_gb);
//     strcat(graphicsArr,diff_virtArr);
//     strcat(memArr[i],graphicsArr);
// }


///for total cpu useage, ask to TA tmmr.


int display_Third_Info(){
    
    /*recall:ut_user:user login name.
    ut_line:device name(console)
    ut_type:type of entry.*/
    setutent(); //to open utmp file;
    int userLine_count=0;
    struct utmp *utmp_info;
   
    printf("### Sessions/users ###\n");
    //getutent(---------------------------------------); :read the next enty from the user -inform file.
    while((utmp_info = getutent()) != NULL){ //read untill null.
        if(utmp_info->ut_type==USER_PROCESS){ //check ut_type is normal process.
            printf("%s\t%s\t", utmp_info->ut_user,utmp_info->ut_line);

            if(*(utmp_info->ut_host)){ // if ut_host, then print ip or given info.
                printf("(%s)\n",utmp_info->ut_host);
            }
            else{
                printf("\n");
            }
            userLine_count++;
        }
       
    }
    endutent(); //closes the internal stream of the utmp database
    return userLine_count;
}
    

void display_cores(){
    int num_cpu = sysconf(_SC_NPROCESSORS_ONLN); //sysconf returns the number of processors in <unistd.h>
    printf("Number of cores: %d\n",num_cpu);
     //print num_cores
}
///for total cpu useage, ask to TA tmmr.
void store_cpuArr(unsigned long curr_cpu[7]) {
    FILE *fp = fopen("/proc/stat", "r");

    if (!fp) {
        perror("Error for opening /proc/stat");
        exit(EXIT_FAILURE);
    }

    
    if (fscanf(fp, "cpu %lu %lu %lu %lu %lu %lu %lu", 
    &curr_cpu[0], &curr_cpu[1], &curr_cpu[2], &curr_cpu[3], &curr_cpu[4], &curr_cpu[5], &curr_cpu[6]) != 7){ 
        fprintf(stderr, "Error reat.ding CPU values\n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    fclose(fp); //mendatory
}

double calculate_cpu_usage(unsigned long prev_cpu[7], unsigned long curr_cpu[7]) {
    unsigned long idle_prev = prev_cpu[3] + prev_cpu[4]; // idle_prev = idle+iowait. i wrote clearly on notion.
    unsigned long idle_cur = curr_cpu[3] + curr_cpu[4]; //similarly,

    unsigned long total_prev = 0;
    unsigned long total_cur = 0;
    for (int i = 0; i < 7; ++i) {
        total_prev += prev_cpu[i];
        total_cur += curr_cpu[i];
    }

    double total_diff = (double)(total_cur - total_prev);
    double idle_diff = (double)(idle_cur - idle_prev);

    return (total_diff - idle_diff) / total_diff * 100.0; // for persentage converted.// from the internet. need confirm.
}

void display_Last_Info(){
    struct utsname sysinfo;
    FILE *uptime_file;
    double uptime_secs;
    uptime_file = fopen("/proc/uptime","r"); //read file uptime.
    fscanf(uptime_file, "%lf", &uptime_secs);
    fclose(uptime_file);

    // Convert to days, hours, minutes, and seconds
    int days = uptime_secs / (24 * 3600);
    uptime_secs = uptime_secs - (days * 24 * 3600);
    int hours = uptime_secs / 3600;
    uptime_secs = uptime_secs - (hours * 3600);
    int minutes = uptime_secs / 60;
    int seconds = (int)uptime_secs % 60;
    int days_to_hr = 24*days;
    int Total_hr = days_to_hr + hours; 
    if(uname(&sysinfo) == 0 && uptime_file != NULL){ // if 정보를 가져오는데 성공하면.
        printf("### System Information ###\n");
        printf("System Name =%s\n",sysinfo.sysname );
        printf("Machine Name=%s\n", sysinfo.nodename);
        printf("Version=%s\n", sysinfo.version);
        printf("Release=%s\n", sysinfo.release);
        printf("Architecture=%s\n", sysinfo.machine);
        printf("System running since last reboot: %d days %02d:%02d:%02d (%02d:%02d:%02d)\n",
         days, hours, minutes, seconds,Total_hr,minutes,seconds);
        
    }
    else{
        //printf("damn really ");
        perror("Error opening /proc/uptime");
        return;
    }
}