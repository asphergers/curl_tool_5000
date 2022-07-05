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

int nums[] = {0,0,0,0,0};

CURL *build(int argc, char** argv, CURL *curl, struct curl_slist *header) {
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

        if (res != CURLE_OK) {
            printf("%s \n", curl_easy_strerror(res));
            continue;
        }
        
        nums[threadNumber]++;
    } 
}

int get_threads(int argc, char** argv) {
    const char* inputs[argc-1];
    for (int i = 0; i<argc-1; i++) {
        inputs[i] = argv[i+1];        
    }

    for (int i = 0; i<argc-1; i++) {
        if (strcmp(inputs[i], "-threads") == 0) {
            //printf("%d \n", atoi(inputs[i+1]));
            return atoi(inputs[i+1]);
        }
    }
    return 1;
}

void *num_manager() {
    while (1>0) {
        printf("\r %d %d %d %d %d", nums[0], nums[1], nums[2], nums[3], nums[4]);
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
        
        pthread_create(&output, NULL, num_manager, NULL);

        pthread_exit(NULL);
    }
    curl_global_cleanup();
    return 0;
}
