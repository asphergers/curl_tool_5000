#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
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
int return_codes[MAX_THREADS];

void set_input_size(int len) {
    argc = len;
}

size_t function_pt(void *ptr, size_t size, size_t nmemb, void *stream){
    ("%d", atoi(ptr));
    return size * nmemb;
}


CURL *build(char* inputs[], CURL *curl, struct curl_slist *header, int post_type) {

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

    if (post_type > 19 && post_type < 30) {
        if (post_type != 23) {
            char* post_data;
            for (int i = 0; i<argc; i++) {
                if (strcmp(inputs[i], "--data-raw") == 0) {
                    printf("%s", post_data);
                    break;
                }
            }
            if (post_type < 23) {
                if (post_type == 21) {
                    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
                } else if (post_type == 22) {
                    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
                }

                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);

            } else {
                printf("invalid post code \n");
            }

        } else {
            curl_easy_setopt(curl, CURLOPT_POST, 2L);
        }
    
    } else if (post_type > 9 && post_type < 11) {
        if (post_type != 10) {
            ;
        } else {
            ;
        }
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, function_pt);

    return curl;
}

void print_request_info(char* inputs[]) {
    printf("-----------HEADERS-----------\n");
    for (int i = 0; i<argc; i++) {
        if (strcmp(inputs[i], "-H") == 0) {
            printf("%s\n", inputs[i+1]);
            i++;
        }
    }
    printf("--------POST FIELDS-------\n");
    for (int i = 0; i<argc; i++) {
        if (strcmp(inputs[i], "--data-raw") == 0) {
            printf("%s\n", inputs[i+1]);
        }
    }
    printf("-----------URL-----------\n");
    for (int i = 0; i<argc; i++) {
        if (strstr(inputs[i], "http") != NULL) {
            printf("%s\n", inputs[i]);
            break;
        }
    }
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
            return_codes[threadNumber] = http_code;
            sleep(15);
            continue;
        }

        sleep(rate);   
        nums[threadNumber]++;
        return_codes[threadNumber] = http_code;
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
            printf("%d: [%d] ", nums[i], return_codes[i]);
        }
        printf("\r");
        fflush(stdout);
    }
}

void init_curls(char** argv, struct curl_slist *header, int threads, CURL *curl[], long request_type) {
    for (int i = 0; i<threads; i++) {
        curl[i] = curl_easy_init();
        curl[i] = build(argv, curl[i], header, request_type);
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

int get_request_type(char* inputs[]) {
    for (int i = 0; i<argc-1; i++) {
        if (strcmp(inputs[i], "-type") == 0) {
            return atoi(inputs[i+1]);
        }
    }
    return 20;
}

void handle_request(char** argv) {
    int threads = get_threads(argv);
    int cycles = get_cycles(argv);
    int rate = get_rate(argv); 
    int request_type = get_request_type(argv);
    pthread_t thread_arr[threads];
    pthread_t output;
    CURL *curl[threads];
    struct curl_slist *header = NULL;
    struct request_loop_args args[threads];
    init_curls(argv, header, threads, curl, request_type);
    init_args(curl, threads, cycles, rate, args); 

    for (int i = 0; i<threads; i++) {
        pthread_create(&thread_arr[i], NULL, request_loop, (void *)&args[i]);
    }

    pthread_create(&output, NULL, num_manager, (void *)&threads);
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    set_input_size(argc);
    
    handle_request(argv);

    return 0;
}