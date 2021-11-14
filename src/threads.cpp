#include <string>
#include <regex>
#include <stdint.h>
#include <unistd.h>
#include <curl/curl.h>

extern "C" {
#include <strutils.h>
}

#include "configmgr.h"
#include "logger.h"
#include "ip_error.h"
#include "threads.h"

using namespace std;

size_t CurlGet_CallbackFunc(void * contents, size_t size, size_t nmemb, std::string * s)
{
    size_t newLength = size * nmemb;

	Logger & log = Logger::getInstance();

    try {
        s->append((char*)contents, newLength);
    }
    catch(std::bad_alloc & e) {
        log.logError("Failed to grow string");
        return 0;
    }

    return newLength;
}

void ThreadManager::startThreads()
{
	Logger & log = Logger::getInstance();

	this->pIPDiscoveryThread = new IPDiscoveryThread();
	if (this->pIPDiscoveryThread->start()) {
		log.logStatus("Started IPDiscoveryThread successfully");
	}
	else {
		throw ip_error("Failed to start IPDiscoveryThread", __FILE__, __LINE__);
	}
}

void ThreadManager::killThreads()
{
	if (this->pIPDiscoveryThread != NULL) {
		this->pIPDiscoveryThread->stop();
	}
}

void * IPDiscoveryThread::run()
{
	bool			go = true;
	char			szIPAddr[32];
	char *			pszUpdateURL;
	const char *	pszUpdateBase;
	const char *	pszUsername;
	const char *	pszPassword;
	string			response;
	string			updateResponse;
	regex 			r("(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)");
	smatch 			m;

	Logger & log = Logger::getInstance();
	ConfigManager & cfg = ConfigManager::getInstance();

	pszUsername = cfg.getValue("update.username");
	pszPassword = cfg.getValue("update.password");

	pszUpdateBase = "https://api.dynu.com/nic/update?username=%s&myip=%s&password=%s";

	pszUpdateURL = (char *)malloc(strlen(pszUpdateBase) + strlen(pszUsername) + strlen(pszPassword) + 16);

	if (pszUpdateURL == NULL) {
		log.logFatal("Failed to allocate memory for update URL");
		exit(-1);
	}

	while (go) {
		CURL * curl = curl_easy_init();

		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, cfg.getValue("ip.discoveryurl"));
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
			curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
			curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
			curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
			
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlGet_CallbackFunc);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
					
			curl_easy_perform(curl);
			
			curl_easy_cleanup(curl);
			
			curl = NULL;
		}

		/*
		** Find the public ip address...
		*/
		if (regex_search(response, m, r)) {
			strcpy(szIPAddr, m.str(0).c_str());
			log.logStatus("Found IP address: %s", szIPAddr);

			sprintf(pszUpdateURL, pszUpdateBase, pszUsername, szIPAddr, pszPassword);

			log.logDebug("Update IP url = %s", pszUpdateURL);
		}

		curl = curl_easy_init();

		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, pszUpdateURL);
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
			curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
			curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
			curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
			
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlGet_CallbackFunc);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &updateResponse);
					
			curl_easy_perform(curl);
			
			curl_easy_cleanup(curl);
			
			curl = NULL;
		}

		log.logStatus("Update service responded with: %s", updateResponse.c_str());

		PosixThread::sleep(PosixThread::minutes, 5);
	}

	free(pszUpdateURL);

	return NULL;
}
