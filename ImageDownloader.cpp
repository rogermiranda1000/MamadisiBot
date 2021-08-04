#include "ImageDownloader.h"

/**
  * Code from https://stackoverflow.com/a/36702936/9178470
  */
size_t ImageDownloader::callbackfunction(void *ptr, size_t size, size_t nmemb, void* userdata) {
    FILE* stream = (FILE*)userdata;
    if (!stream) {
        std::cerr << "No stream!" << std::endl;
        return 0;
    }

    return fwrite((FILE*)ptr, size, nmemb, stream);
}

/**
  * Code from https://stackoverflow.com/a/36702936/9178470
  */
bool ImageDownloader::download_jpeg(const char *path, const char *url) {
	if(access(path, F_OK) == 0) return false; // file already exists
	
    FILE* fp = nullptr;
	fp = fopen(path, "wb");
    if (fp == nullptr) {
        std::cerr << "Failed to create file on the disk!" << std::endl;
        return false;
    }

    CURL* curlCtx = curl_easy_init();
    curl_easy_setopt(curlCtx, CURLOPT_URL, url);
    curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, ImageDownloader::callbackfunction);
    curl_easy_setopt(curlCtx, CURLOPT_FOLLOWLOCATION, 1);

    CURLcode rc = curl_easy_perform(curlCtx);
    if (rc) {
        std::cerr << "Failed to download [" << url << "]" << std::endl;
        return false;
    }

    long res_code = 0;
    curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &res_code);
    if (!((res_code == 200 || res_code == 201) && rc != CURLE_ABORTED_BY_CALLBACK)) {
        std::cerr << "Response code: " << res_code << std::endl;
        return false;
    }

    curl_easy_cleanup(curlCtx);

    fclose(fp);

    return true;
}

std::string ImageDownloader::gen_random(const int len) {
    std::string tmp_s;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    
    srand( (unsigned) time(NULL) * getpid());

    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) 
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    
    
    return tmp_s;
}