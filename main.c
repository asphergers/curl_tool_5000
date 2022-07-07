#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<curl/curl.h>
#include<pthread.h>

#define MAX_THREADS 30

struct request_loop_args {
    CURL *curl;
    int threadNumber;
    int cycles;
    float rate;
};

int argc;
int nums[MAX_THREADS];

void set_input_size(int len) {
    argc = len;
}

size_t function_pt(void *ptr, size_t size, size_t nmemb, void *stream){
    //("%d", atoi(ptr));
    return size * nmemb;
}


CURL *build(char* inputs[], CURL *curl, struct curl_slist *header) {

    for (int i = 0; i<argc; i++) {
        if (strcmp(inputs[i], "-H") == 0) {
            header = curl_slist_append(header, inputs[i+1]);
            i++;
        }
    }

    for (int i = 0; i<argc; i++) {
        if (strstr(inputs[i], "http") != NULL) {
            curl_easy_setopt(curl, CURLOPT_URL, inputs[i]);
            break;
        }
    }

    for (int i = 0; i<argc; i++) {
        if (strcmp(inputs[i], "--data-raw") == 0) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, inputs[i+1]);
            break;
        }  
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, function_pt);

    return curl;
}

void* request_loop(void *arguments) {
    struct request_loop_args *args = arguments;

    int threadNumber = args->threadNumber;
    CURL *curl = args->curl;
    int cycles = args->cycles;
    float rate = args->rate;

    CURLcode res;
    while (cycles != 0) {
        res = curl_easy_perform(curl);
        long http_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        if (res != CURLE_OK) {
            printf("%s \n", curl_easy_strerror(res));
            continue;
        }

        if (http_code == 429) {
            nums[threadNumber]--;
            sleep(15);
        }
        sleep(rate);   
        nums[threadNumber]++;
        cycles--;
    } 
    exit(0);
}

int get_cycles(char* inputs[]) {
    for (int i = 0; i<argc-1; i++) {
        if (strcmp(inputs[i], "-cycles") == 0) {
            return atoi(inputs[i+1]);
        } 
    }

    return -1;
}

int get_threads(char* inputs[]) {
    for (int i = 0; i<argc-1; i++) {
        if (strcmp(inputs[i], "-threads") == 0) {
            if (atoi(inputs[i+1]) > MAX_THREADS) {
                printf("AMOUNT OF THREADS MUST NOT EXCEED %d", MAX_THREADS);
                exit(0);
            }
            return atoi(inputs[i+1]);
        }
    }
    return 1;
}

float get_rate(char* inputs[]) {
    for (int i = 0; i<argc-1; i++) {
        if (strcmp(inputs[i], "-rate") == 0) {
            return atof(inputs[i+1]);
        }
    }
    return 0.0;
}

void *num_manager(void *thds) {
   int threads = *(int *)thds;
   printf("%d \n", threads);
    while (1>0) {
        for (int i = 0; i<threads; i++) {
            printf("%d  ", nums[i]);
        }
        printf("\r");
        fflush(stdout);
    }
}

void init_curls(char** argv, struct curl_slist *header, int threads, CURL *curl[]) {
    for (int i = 0; i<threads; i++) {
        curl[i] = curl_easy_init();
        curl[i] = build(argv, curl[i], header);
    }
}

void init_args(CURL *curl[], int threads, int cycles, float rate, struct request_loop_args args[]) {
    for (int i = 0; i<threads; i++) {
        args[i].curl = curl[i];
        args[i].threadNumber = i;
        args[i].cycles = cycles;
        args[i].rate = rate;
    }
}


int main(int argc, char** argv) {

    set_input_size(argc);
    int threads = get_threads(argv); 
    int cycles = get_cycles(argv);
    float rate = get_rate(argv);
    pthread_t thread_arr[threads];
    pthread_t output;
    CURL *curl[threads];
    CURLcode res;
    struct curl_slist *header = NULL;
    struct request_loop_args args[threads];
    init_curls(argv, header, threads, curl);
    init_args(curl, threads, cycles, rate, args);
    //printf("%p \n", curl[0]);

    if(curl) {
        for (int i = 0; i < threads; i++) {
            pthread_create(&thread_arr[i], NULL, request_loop, (void *)&args[i]);
        }
        pthread_create(&output, NULL, num_manager, (void *)&threads);
        pthread_exit(NULL);
    }
    curl_global_cleanup();
    return 0;
}

