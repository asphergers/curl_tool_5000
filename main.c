#include<stdio.h>
#include<string.h>
#include<curl/curl.h>



int main(int argc, char** argv) {
    const char *inputs[argc-1];
    for(int i = 0; i<argc-1; i++) {
        inputs[i] = argv[i+1];    
    }
    CURL *curl;
    CURLcode res;
    struct curl_slist *header = NULL;
    curl = curl_easy_init();
    if(curl) {
        // setting the curl request headers
        for (int i = 0; i<argc; i++) {
            if (strcmp(inputs[i], "-H") == 0) {
                header = curl_slist_append(header, inputs[i+1]);
                i++;
            }
        }
        // finding and setting the url
        for (int i = 0; i<argc; i++) {
            if (strstr(inputs[i], "http") != NULL) {
                curl_easy_setopt(curl, CURLOPT_URL, inputs[i]);
                break;
            }
        }
        // getting and setting the post data
        for (int i = 0; i<argc; i++) {
            if (strcmp(inputs[i], "--data-raw") == 0) {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, inputs[i+1]);
                break;
            }
        }
        int count = 0;
        printf("\n\n");
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
        while (1>0) {
            res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                //curl_easy_cleanup(curl);
            };
            count++;
            printf("\r %d", count);   
            fflush(stdout);

        }
    }
    curl_global_cleanup();
    return 0;
}
