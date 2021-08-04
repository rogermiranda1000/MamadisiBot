#pragma once

#include <stdio.h> // fwrite
#include <unistd.h> // check if file exists
#include <iostream> // print the error messages
#include <curl/curl.h> // download from website

#define RANDOM_TEXT_LEN 128

class ImageDownloader {
public:
	static bool download_jpeg(const char *path, const char *url);
	static std::string gen_random(const int len = RANDOM_TEXT_LEN);
	
private:
	static size_t callbackfunction(void *ptr, size_t size, size_t nmemb, void* userdata);
};