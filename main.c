#include<stdio.h>
#include<string.h>
#include<curl/curl.h>

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

int requestLoop(CURL *curl) {
   CURLcode res;
   int count = 0;
   while (1 > 0) {
       res = curl_easy_perform(curl);

       if (res != CURLE_OK) {
           printf("%s \n", "ran into an issue");
           continue;
       }

       count++;
       printf("\r %d", count);
       fflush(stdout);
   } 
   return 0;
}

int main(int argc, char** argv) {
    CURL *curl;
    CURLcode res;
    struct curl_slist *header = NULL;
    curl = curl_easy_init();
    if(curl) {        
        curl = build(argc, argv, curl, header);
        requestLoop(curl);
    }
    curl_global_cleanup();
    return 0;
}
