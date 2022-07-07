#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<curl/curl.h>
#include<pthread.h>

struct request_loop_args {
    CURL *curl;
    int threadNumber;
};

int nums[30];


size_t function_pt(void *ptr, size_t size, size_t nmemb, void *stream){
    //("%d", atoi(ptr));
    return size * nmemb;
}


CURL *build(int argc, char** argv, CURL *curl, struct curl_slist *header) {
    size_t header_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
    const char *inputs[argc-1];
    for (int i = 0; i<argc-1; i++) {
        inputs[i] = argv[i+1];
    }

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

    CURLcode res;
    int count = 0;
    while (1 > 0) {
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
        
        nums[threadNumber]++;
        sleep(0.900);
    } 
}

int get_threads(int argc, char** argv) {
    const char* inputs[argc-1];
    for (int i = 0; i<argc-1; i++) {
        inputs[i] = argv[i+1];        
    }

    for (int i = 0; i<argc-1; i++) {
        if (strcmp(inputs[i], "-threads") == 0) {
            if (atoi(inputs[i+1]) > 30) {
                printf("AMOUNT OF THREADS MUST NOT EXCEED 30");
                exit(0);
            }
            return atoi(inputs[i+1]);
        }
    }
    return 1;
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

void init_curls(int argc, char** argv, struct curl_slist *header, int threads, CURL *curl[]) {
    for (int i = 0; i<threads; i++) {
        curl[i] = curl_easy_init();
        curl[i] = build(argc, argv, curl[i], header);
    }
}

void init_args(CURL *curl[], int threads, struct request_loop_args args[]) {
    for (int i = 0; i<threads; i++) {
        args[i].curl = curl[i];
        args[i].threadNumber = i;
    }
}

int main(int argc, char** argv) {

    int threads = get_threads(argc, argv); 
    pthread_t thread_arr[threads];
    pthread_t output;
    CURL *curl[threads];
    CURLcode res;
    struct curl_slist *header = NULL;
    struct request_loop_args args[threads];
    init_curls(argc, argv, header, threads, curl);
    init_args(curl, threads, args);
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
