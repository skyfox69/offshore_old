#include "urlloader.h"
#include <curl/curl.h>
#include <sstream>
#include <fstream>

//-----------------------------------------------------------------------------
static size_t dataWrite(void* pBuf, size_t size, size_t nmemb, void* pUser)
{
	if (pUser != nullptr) {
		ostream&	os (*static_cast<ostream*>(pUser));
		streamsize	len(size * nmemb);

		if (os.write(static_cast<char*>(pBuf), len)) {
			return len;
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------
UrlLoader*	UrlLoader::_pInstance = nullptr;

//-----------------------------------------------------------------------------
UrlLoader::UrlLoader()
{
	curl_global_init(CURL_GLOBAL_ALL);
}

//-----------------------------------------------------------------------------
UrlLoader::~UrlLoader()
{
	curl_global_cleanup();
}

//-----------------------------------------------------------------------------
UrlLoader* UrlLoader::getInstance()
{
	if (_pInstance == nullptr) {
		_pInstance = new UrlLoader();
	}
	return _pInstance;
}

//-----------------------------------------------------------------------------
bool UrlLoader::read(const string url, ostream& oStream)
{
	//  init curl
	CURL*		pCurl  (curl_easy_init());
	CURLcode	code   (CURLE_FAILED_INIT);
	bool		retCode(false);

	//  if curl init succeeded
	if (pCurl != nullptr) {
		//  set curl options
		if ((CURLE_OK == (code = curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, &dataWrite))) &&
			(CURLE_OK == (code = curl_easy_setopt(pCurl, CURLOPT_NOPROGRESS, 1L))) &&
			(CURLE_OK == (code = curl_easy_setopt(pCurl, CURLOPT_FOLLOWLOCATION, 1L))) &&
			(CURLE_OK == (code = curl_easy_setopt(pCurl, CURLOPT_FILE, &oStream))) &&
			(CURLE_OK == (code = curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 300))) &&
			(CURLE_OK == (code = curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str())))
		   ) {
			retCode = (CURLE_OK == (code = curl_easy_perform(pCurl)));
		}

		//  clean up curl
		curl_easy_cleanup(pCurl);

	}  //  if (pCurl != nullptr)

	return retCode;
}

//-----------------------------------------------------------------------------
string UrlLoader::readHtml(const string url)
{
	ostringstream	oStream;

	if (!read(url, oStream)) {
		oStream.str("");
	}
	return oStream.str();
}

//-----------------------------------------------------------------------------
string UrlLoader::downloadImage(const string url, const string fileName)
{
	ofstream	oStream(fileName, ofstream::binary);

	read(url, oStream);
	oStream.flush();
	oStream.close();
	return fileName;
}