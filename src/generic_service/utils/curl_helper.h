#pragma once

#include <errno.h>
#include <string.h>
#include <filesystem>
#include <curl/curl.h>
#include <curl/multi.h>

//#define USERAGENT_DEFAULT "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4044.113 Safari/537.36 Edg/81.0.416.58"
#define USERAGENT_DEFAULT "PPSCURLTOOL/1.9.9.9"

#define CURL_GLOBAL_INIT(X) curl_global_init(X)

#if !defined(OPENSSL_VERSION_1_1)
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/engine.h>
#include <openssl/ssl.h>
#define CURL_GLOBAL_EXIT() { \
curl_global_cleanup(); \
CONF_modules_free(); \
ERR_remove_state(0); \
ENGINE_cleanup(); \
CONF_modules_unload(1); \
ERR_free_strings(); \
EVP_cleanup(); \
CRYPTO_cleanup_all_ex_data(); \
sk_SSL_COMP_free(SSL_COMP_get_compression_methods()); \
}
#else
#define CURL_GLOBAL_EXIT() curl_global_cleanup()
#endif

typedef struct _tagByteData {
#define call_back_data_size 0xffff
	char* p;
	unsigned int s;
	unsigned int v;
	static struct _tagByteData* startup()
	{
		struct _tagByteData* thiz = (struct _tagByteData*)malloc(sizeof(struct _tagByteData));
		if (thiz)
		{
			thiz->init();
		}
		return thiz;
	}
	_tagByteData()
	{
	}
	_tagByteData(char** _p, unsigned int _s = 0, unsigned int _v = 0)
	{
		init(_p, _s, _v);
	}
	void init(char** _p = 0, unsigned int _s = 0, unsigned int _v = 0)
	{
		p = _p ? (*_p) : 0; s = _s; v = (!p || !_v) ? call_back_data_size : _v;
		if (!p)
		{
			p = (char*)malloc(v * sizeof(char));
		}
		if (p && v > 0)
		{
			memset(p, 0, v * sizeof(char));
		}
	}
	void copy(const char* _p, unsigned int _s)
	{
		if (_s > 0)
		{
			if (s + _s > v)
			{
				v += _s + 1;
				p = (char*)realloc(p, v * sizeof(char));
				memset(p + s, 0, _s + 1);
			}
			if (p)
			{
				memcpy(p, _p, _s);
				s = _s;
			}
		}
	}
	char* append(char* _p, unsigned int _s)
	{
		if (_s > 0)
		{
			if (s + _s > v)
			{
				v += _s + 1;
				p = (char*)realloc(p, v * sizeof(char));
				memset(p + s, 0, _s + 1);
			}
			if (p)
			{
				memcpy(p + s, _p, _s);
				s += _s;
			}
		}
		return p;
	}
	void clear()
	{
		if (p)
		{
			memset(p, 0, s);
			s = 0;
		}
	}
	void exit(char** _p)
	{
		if (_p && (*_p))
		{
			free((*_p));
			(*_p) = 0;
		}
		s = v = 0;
	}
	void cleanup()
	{
		exit(&p);
		free(this);
	}
}bytedata, * pbytedata, BYTEDATA, * PBYTEDATA;

//curl 回调处理返回数据函数
__inline static 
size_t write_native_data_callback(void* p_data, size_t n_size, size_t n_menb, void* p_argv)
{
	pbytedata pcbd = (pbytedata)p_argv;
	if (pcbd && p_data)
	{
		pcbd->append((char*)p_data, n_size * n_menb);
	}
	return n_size * n_menb;
}

//////////////////////////////////////////////////////////////////////
//函数功能:传入URL获取JSON字符串
//函数参数:
//		pByteData			返回的JSON数据字符串
//		pRequestUrl			下载地址URL
//		bPostRequest		是否为POST请求
//		pHeaderDataList		要发送的头部数据{"A:B","C:D"}
//		pPostField			发送的POST域数据
//		nProxyType			设置代理类型(CURLPROXY_HTTP, CURLPROXY_HTTPS, CURLPROXY_SOCKS4, CURLPROXY_SOCKS4A, CURLPROXY_SOCKS5)
//		pProxyPort			设置请求使用的代理([host]:[port])
//		pProxyUserPwd		设置代理需要的用户密码(user:password)
//		bVerbose			是否为详细日志信息
//		nDelayTime			超时设置，默认为0毫秒
//返回值:
//		CURLcode
__inline static 
CURLcode curl_http_form_execute(
	pbytedata pByteData,
	const char* pRequestUrl,
	const char* pCustomRequest = "GET",
	std::list<std::string>* pHeaderDataList = 0,
	const char* pPostField = 0,
	curl_proxytype nProxyType = CURLPROXY_HTTP,
	const char* pProxyPort = 0,
	const char* pProxyUserPwd = 0,
	bool bVerbose = false,
	int nDelayTime = 0)
{
	CURL* pCurl = 0;
	CURLcode curlCode = CURLE_OK;
	struct curl_slist* plist = 0;
	struct curl_httppost* formpost = 0;
	struct curl_httppost* lastptr = 0;

	pCurl = curl_easy_init();
	if (pCurl)
	{
		// transfer data in text / ASCII format
		//curl_easy_setopt(pCurl, CURLOPT_TRANSFERTEXT, 1L);

		// use Location: Luke!
		//curl_easy_setopt(pCurl, CURLOPT_FOLLOWLOCATION, 1L);

		// We want the referrer field set automatically when following locations
		curl_easy_setopt(pCurl, CURLOPT_AUTOREFERER, 1L);

		// Set to explicitly forbid the upcoming transfer's connection to be re-used
		// when done. Do not use this unless you're absolutely sure of this, as it
		// makes the operation slower and is less friendly for the network. 
		curl_easy_setopt(pCurl, CURLOPT_FORBID_REUSE, 0L);

		// Instruct libcurl to not use any signal/alarm handlers, even when using
		// timeouts. This option is useful for multi-threaded applications.
		// See libcurl-the-guide for more background information. 
		curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1L);

		// send it verbose for max debuggaility
		if (bVerbose)
		{
			curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L);
		}

		// HTTP/2 please
		curl_easy_setopt(pCurl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);

		// we use a self-signed test server, skip verification during debugging
		curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, 2L);
		curl_easy_setopt(pCurl, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);

		// Function that will be called to store the output (instead of fwrite). The
		// parameters will use fwrite() syntax, make sure to follow them.
		//curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, [](void * p_data, size_t n_size, size_t n_menb, void * p_argv){});
		curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, write_native_data_callback);
		curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void*)pByteData);

		//char *output = curl_easy_escape(pCurl, strRequestURL.c_str(), strRequestURL.length());
		curl_easy_setopt(pCurl, CURLOPT_TCP_KEEPALIVE, 1L);

		// The full URL to get/put
		curl_easy_setopt(pCurl, CURLOPT_URL, pRequestUrl);

		// Set User-Agent
		curl_easy_setopt(pCurl, CURLOPT_USERAGENT, USERAGENT_DEFAULT);

		//设置头部数据
		if (pHeaderDataList)
		{
			// This points to a linked list of headers, struct curl_slist kind. This
			// list is also used for RTSP (in spite of its name)
			for (auto it = pHeaderDataList->begin(); it != pHeaderDataList->end(); it++)
			{
				plist = curl_slist_append(plist, it->c_str());
			}
			curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, plist);
		}

		// HTTP POST method
		curl_easy_setopt(pCurl, CURLOPT_CUSTOMREQUEST, pCustomRequest);

		// Now specify the POST data
		//设置POST域数据
		curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, pPostField);

		if (nDelayTime > 0)
		{
			// Time-out the read operation after this amount of milliseconds
			curl_easy_setopt(pCurl, CURLOPT_TIMEOUT_MS, nDelayTime);
		}

		if (pProxyPort)
		{
			// indicates type of proxy. accepted values are CURLPROXY_HTTP (default),
			// CURLPROXY_HTTPS, CURLPROXY_SOCKS4, CURLPROXY_SOCKS4A and
			// CURLPROXY_SOCKS5.
			curl_easy_setopt(pCurl, CURLOPT_PROXYTYPE, nProxyType);

			// Port of the proxy, can be set in the proxy string as well with: "[host]:[port]"
			curl_easy_setopt(pCurl, CURLOPT_PROXYPORT, pProxyPort);

			if (pProxyUserPwd)
			{
				// "user:password" to use with proxy.
				curl_easy_setopt(pCurl, CURLOPT_PROXYUSERPWD, pProxyUserPwd);
			}
		}

		curlCode = curl_easy_perform(pCurl);
		if (plist != nullptr)
		{
			curl_slist_free_all(plist);
		}
		curl_easy_cleanup(pCurl);
	}

	return curlCode;
}

//////////////////////////////////////////////////////////////////////
//函数功能:传入URL获取JSON字符串
//函数参数:
//		pByteData			返回的JSON数据字符串
//		pRequestUrl			下载地址URL
//		pCustomRequest		请求方式
//		pHeaderDataList		要发送的头部数据{"A:B","C:D"}
//		pFormFieldMap		发送的FORM键值对列表(如若是文件，则值路径间隔符为'/')
//		pPostFileds			发送的POST域数据
//		nProxyType			设置代理类型(CURLPROXY_HTTP, CURLPROXY_HTTPS, CURLPROXY_SOCKS4, CURLPROXY_SOCKS4A, CURLPROXY_SOCKS5)
//		pProxyPort			设置请求使用的代理([host]:[port])
//		pProxyUserPwd		设置代理需要的用户密码(user:password)
//		bVerbose			是否为详细日志信息
//		nDelayTime			超时设置，默认为0毫秒
//返回值:
//		CURLcode
__inline static 
CURLcode curl_http_multform_execute(
	pbytedata pByteData,
	const char* pRequestUrl,
	const char* pCustomRequest = "GET",
	std::list<std::string>* pHeaderDataList = 0,
	std::map<std::string, std::string>* pFormFieldMap = 0,
	const char* pPostFileds = 0,
	curl_proxytype nProxyType = CURLPROXY_HTTP,
	const char* pProxyPort = 0,
	const char* pProxyUserPwd = 0,
	bool bVerbose = false,
	int nDelayTime = 0)
{
	CURL* pCurl = 0;
	CURLcode curlCode = CURLE_OK;
	struct curl_slist* plist = 0;
	struct curl_httppost* formpost = 0;
	struct curl_httppost* lastptr = 0;

	pCurl = curl_easy_init();
	if (pCurl)
	{
		// transfer data in text / ASCII format
		//curl_easy_setopt(pCurl, CURLOPT_TRANSFERTEXT, 1L);

		// use Location: Luke!
		//curl_easy_setopt(pCurl, CURLOPT_FOLLOWLOCATION, 1L);

		// We want the referrer field set automatically when following locations
		curl_easy_setopt(pCurl, CURLOPT_AUTOREFERER, 1L);

		// Set to explicitly forbid the upcoming transfer's connection to be re-used
		// when done. Do not use this unless you're absolutely sure of this, as it
		// makes the operation slower and is less friendly for the network. 
		curl_easy_setopt(pCurl, CURLOPT_FORBID_REUSE, 0L);

		// Instruct libcurl to not use any signal/alarm handlers, even when using
		// timeouts. This option is useful for multi-threaded applications.
		// See libcurl-the-guide for more background information. 
		curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1L);

		// send it verbose for max debuggaility
		if (bVerbose)
		{
			curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L);
		}

		// HTTP/2 please
		curl_easy_setopt(pCurl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);

		// we use a self-signed test server, skip verification during debugging
		curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, 2L);
		curl_easy_setopt(pCurl, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);

		// Function that will be called to store the output (instead of fwrite). The
		// parameters will use fwrite() syntax, make sure to follow them.
		//curl_easy_setopt(p_curl, CURLOPT_WRITEFUNCTION, [](void * p_data, size_t n_size, size_t n_menb, void * p_argv){});
		curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, write_native_data_callback);
		curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void*)pByteData);

		//char *output = curl_easy_escape(pCurl, strRequestURL.c_str(), strRequestURL.length());
		curl_easy_setopt(pCurl, CURLOPT_TCP_KEEPALIVE, 1L);

		// The full URL to get/put
		curl_easy_setopt(pCurl, CURLOPT_URL, pRequestUrl);

		// Set User-Agent
		curl_easy_setopt(pCurl, CURLOPT_USERAGENT, USERAGENT_DEFAULT);

		//设置头部数据
		if (pHeaderDataList)
		{
			// This points to a linked list of headers, struct curl_slist kind. This
			// list is also used for RTSP (in spite of its name)
			for (auto it = pHeaderDataList->begin(); it != pHeaderDataList->end(); it++)
			{
				plist = curl_slist_append(plist, it->c_str());
			}
			curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, plist);
		}

		// HTTP POST method
		curl_easy_setopt(pCurl, CURLOPT_CUSTOMREQUEST, pCustomRequest);
		// Now specify the POST data (name=daniel&project=curl)
		curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, pPostFileds);

		if (pFormFieldMap)
		{
			for (auto it = pFormFieldMap->begin(); it != pFormFieldMap->end(); it++)
			{
				if (it->second.rfind('/') != std::string::npos && std::filesystem::exists(it->second))
				{
					// Fill in the file upload field. This makes libcurl load data from
					// the given file name when curl_easy_perform() is called. 
					curl_formadd(&formpost,
						&lastptr,
						CURLFORM_COPYNAME, it->first.c_str(),
						CURLFORM_FILE, it->second.c_str(),
						CURLFORM_FILENAME, it->second.substr(it->second.rfind('/')).c_str(),
						CURLFORM_END);
				}
				else
				{
					// Fill in the filename field
					curl_formadd(&formpost,
						&lastptr,
						CURLFORM_COPYNAME, it->first.c_str(),
						CURLFORM_COPYCONTENTS, it->second.c_str(),
						CURLFORM_END);
				}
			}
			if (formpost)
			{
				// This points to a linked list of post entries, struct curl_httppost
				curl_easy_setopt(pCurl, CURLOPT_HTTPPOST, formpost);
			}
		}

		if (nDelayTime > 0)
		{
			// Time-out the read operation after this amount of milliseconds
			curl_easy_setopt(pCurl, CURLOPT_TIMEOUT_MS, nDelayTime);
		}

		if (pProxyPort)
		{
			// indicates type of proxy. accepted values are CURLPROXY_HTTP (default),
			// CURLPROXY_HTTPS, CURLPROXY_SOCKS4, CURLPROXY_SOCKS4A and CURLPROXY_SOCKS5.
			curl_easy_setopt(pCurl, CURLOPT_PROXYTYPE, nProxyType);

			// Port of the proxy, can be set in the proxy string as well with: "[host]:[port]"
			curl_easy_setopt(pCurl, CURLOPT_PROXYPORT, pProxyPort);

			if (pProxyUserPwd)
			{
				// "user:password" to use with proxy.
				curl_easy_setopt(pCurl, CURLOPT_PROXYUSERPWD, pProxyUserPwd);
			}
		}

		curlCode = curl_easy_perform(pCurl);

		if (formpost)
		{
			curl_formfree(formpost);
		}
		if (plist != nullptr)
		{
			curl_slist_free_all(plist);
		}
		curl_easy_cleanup(pCurl);
	}

	return curlCode;
}

__inline static 
CURLcode CURL_HTTP_STARTUP()
{
	return CURL_GLOBAL_INIT(CURL_GLOBAL_DEFAULT);
}

__inline static 
void CURL_HTTP_CLEANUP()
{
	CURL_GLOBAL_EXIT();
}

//////////////////////////////////////////////////////////////////////

__inline static 
std::string post_form(const std::string& url, std::map<std::string, std::string>& sv)
{
	std::string result = "";
	std::list<std::string> headerList = { "Expect:" };
	CURLcode curlCode = CURLcode::CURLE_OK;
	pbytedata pbd = bytedata::startup();

	curlCode = curl_http_multform_execute(pbd, url.c_str(), "POST", &headerList, &sv);

	if (pbd->p)
	{
		result.assign(pbd->p, pbd->s);
	}
	pbd->cleanup();

	return result;
}

#ifndef CURLPIPE_MULTIPLEX
/* This little trick will just make sure that we don't enable pipelining for
libcurls old enough to not have this symbol. It is _not_ defined to zero in
a recent libcurl header. */
#define CURLPIPE_MULTIPLEX 0
#endif

#define TASKMAP_CLEANUP(T, M)	{ \
	CURLM * pcurlm = (CURLM *)M; \
	std::map<std::string, CTaskInfoData> * ptaskmap = reinterpret_cast<std::map<std::string, CTaskInfoData> *>(T); \
	for (auto it = ptaskmap->begin(); it != ptaskmap->end(); it++) \
	{ \
		if (it->second.curl != nullptr) \
		{ \
			if(pcurlm) \
			{ \
				curl_multi_remove_handle(pcurlm, it->second.curl); \
			} \
			curl_easy_cleanup(it->second.curl); \
			it->second.curl = nullptr; \
		} \
	} \
}

typedef enum TASKSTATETYPE {
	TSTYPE_NULL = 0,//0-任务未启动
	TSTYPE_WAITING = 1,//1-任务等待处理中
	TSTYPE_CANCEL = 2,//2-任务已取消
	TSTYPE_PROCESSING = 3,//3-任务正在处理中
	TSTYPE_FINISH = 4,//4-任务处理完成
	TSTYPE_FAILED = 5,//5-任务处理失败
}TASKSTATETYPE;

class CTaskInfoData
{
public:
	CTaskInfoData(time_t time,
		const std::string& url,
		const std::string& method = "GET",
		curl_slist* header_slist = nullptr,
		const std::string& post_fields = "",
		const std::string& user_agents = USERAGENT_DEFAULT) {
		this->index = 0;
		this->time = time;
		this->curl = nullptr;
		this->url = url;
		this->header_slist = header_slist;
		this->state = TSTYPE_NULL;
		this->method = method;
		this->user_agents = user_agents;
		this->response_data = "";
	}
	virtual ~CTaskInfoData() {}
public:
	int index;
	CURL* curl;
	time_t time;
	std::string url;
	TASKSTATETYPE state;
	std::string method;
	std::string user_agents;
	std::string post_fields;
	std::string response_data;
	curl_slist* header_slist;
};

typedef void(*PFUN_NotifyUpdate)(void* thiz);
typedef int(*PFUN_IsProcessing)(void* thiz);
typedef std::map<time_t, CTaskInfoData>* (*PFUN_GetTaskMap)(void* thiz);

__inline static
void dump(const char* text, int index, unsigned char* ptr, size_t size, char nohex)
{
	size_t i = 0;
	size_t c = 0;

	unsigned int width = 0x10;

	if (nohex)
		/* without the hex output, we can fit more on screen */
		width = 0x40;

	fprintf(stdout, "%d %s, %lu bytes (0x%lx)\n",
		index, text, (unsigned long)size, (unsigned long)size);

	for (i = 0; i < size; i += width) {

		fprintf(stdout, "%4.4lx: ", (unsigned long)i);

		if (!nohex) {
			/* hex not disabled, show it */
			for (c = 0; c < width; c++)
				if (i + c < size)
					fprintf(stdout, "%02x ", ptr[i + c]);
				else
					fprintf(stdout, "   ");
		}

		for (c = 0; (c < width) && (i + c < size); c++) {
			/* check for 0D0A; if found, skip past and start a new line of output */
			if (nohex && (i + c + 1 < size) && ptr[i + c] == 0x0D &&
				ptr[i + c + 1] == 0x0A) {
				i += (c + 2 - width);
				break;
			}
			fprintf(stdout, "%c", (ptr[i + c] >= 0x20) && (ptr[i + c] < 0x80) ? ptr[i + c] : '.');
			/* check again for 0D0A, to avoid an extra \n if it's at width */
			if (nohex && (i + c + 2 < size) && ptr[i + c + 1] == 0x0D &&
				ptr[i + c + 2] == 0x0A) {
				i += (c + 3 - width);
				break;
			}
		}
		fprintf(stdout, "\n"); /* newline */
	}
}

__inline static
int debug_trace(CURL* handle, curl_infotype type, char* data, size_t size, void* userp)
{
	const char* text = nullptr;
	CTaskInfoData* pdtb = reinterpret_cast<CTaskInfoData*>(userp);
	int index = pdtb->index;
	(void)handle; /* prevent compiler warning */

	switch (type) {
	case CURLINFO_TEXT:
		fprintf(stderr, "== %d Info: %s", index, data);
		/* FALLTHROUGH */
	default: /* in case a new one is introduced to shock us */
		return 0;

	case CURLINFO_HEADER_OUT:
		text = "=> Send header";
		break;
	case CURLINFO_DATA_OUT:
		text = "=> Send data";
		break;
	case CURLINFO_SSL_DATA_OUT:
		text = "=> Send SSL data";
		break;
	case CURLINFO_HEADER_IN:
		text = "<= Recv header";
		break;
	case CURLINFO_DATA_IN:
		text = "<= Recv data";
		break;
	case CURLINFO_SSL_DATA_IN:
		text = "<= Recv SSL data";
		break;
	}

	dump(text, index, (unsigned char*)data, size, 1);
	return 0;
}

//curl 回调处理返回数据函数
__inline static
size_t write_string_data_callback(void* p_data, size_t n_size, size_t n_menb, void* p_argv)
{
	std::string* pstr = reinterpret_cast<std::string*>(p_argv);
	if (pstr && p_data)
	{
		pstr->append((const char*)p_data, n_size * n_menb);
	}
	return n_size * n_menb;
}

__inline static
void init_task(CURLM* pCurlM, CTaskInfoData* pTID)
{
	pTID->curl = curl_easy_init();

	// We want the referrer field set automatically when following locations
	curl_easy_setopt(pTID->curl, CURLOPT_AUTOREFERER, 1L);

	// Set to explicitly forbid the upcoming transfer's connection to be re-used
	// when done. Do not use this unless you're absolutely sure of this, as it
	// makes the operation slower and is less friendly for the network. 
	curl_easy_setopt(pTID->curl, CURLOPT_FORBID_REUSE, 0L);

	// Instruct libcurl to not use any signal/alarm handlers, even when using
	// timeouts. This option is useful for multi-threaded applications.
	// See libcurl-the-guide for more background information. 
	curl_easy_setopt(pTID->curl, CURLOPT_NOSIGNAL, 1L);
	//指定请求方式
	curl_easy_setopt(pTID->curl, CURLOPT_CUSTOMREQUEST, pTID->method.data());
	curl_easy_setopt(pTID->curl, CURLOPT_POSTFIELDS, pTID->post_fields.data());
	curl_easy_setopt(pTID->curl, CURLOPT_POSTFIELDSIZE, pTID->post_fields.size());
	curl_easy_setopt(pTID->curl, CURLOPT_HTTPHEADER, pTID->header_slist);//set head
	curl_easy_setopt(pTID->curl, CURLOPT_VERBOSE, 1L);
	//请求的网址
	curl_easy_setopt(pTID->curl, CURLOPT_URL, pTID->url.data());
	/////////////////////////////////////////////////////////////////////////////////////
	//把调用的网址存下
	//	curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &pTID);
	//	查询出来
	curl_easy_setopt(pTID->curl, CURLOPT_PRIVATE, pTID);
	//设置调用cb函数时传递cb函数的第四个参数
	//curl_easy_setopt(pTID->pcurl, CURLOPT_WRITEDATA, pTID->pfile);
	//curl_easy_setopt(pTID->pcurl, CURLOPT_WRITEFUNCTION, write_native_data_callback);
	curl_easy_setopt(pTID->curl, CURLOPT_WRITEFUNCTION, write_string_data_callback);
	curl_easy_setopt(pTID->curl, CURLOPT_WRITEDATA, (void*)&pTID->response_data);

#if defined(_DEBUG) || defined(DEBUG)
	// please be verbose
	curl_easy_setopt(pTID->curl, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(pTID->curl, CURLOPT_DEBUGFUNCTION, debug_trace);
	curl_easy_setopt(pTID->curl, CURLOPT_DEBUGDATA, pTID);
#else
	/* please be verbose */
	curl_easy_setopt(pTID->curl, CURLOPT_VERBOSE, 0L);
#endif

	/* HTTP/1.1 please */
	curl_easy_setopt(pTID->curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);

	/* we use a self-signed test server, skip verification during debugging */
	curl_easy_setopt(pTID->curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(pTID->curl, CURLOPT_SSL_VERIFYHOST, 2L);
	curl_easy_setopt(pTID->curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT);

#if (CURLPIPE_MULTIPLEX > 0)
	/* wait for pipe connection to confirm */
	curl_easy_setopt(pTID->curl, CURLOPT_PIPEWAIT, 1L);
#endif
	//设置useragent
	curl_easy_setopt(pTID->curl, CURLOPT_USERAGENT, pTID->user_agents.data());

	curl_multi_add_handle(pCurlM, pTID->curl);
}

__inline static
int async_start(
	std::string& resp_data, 
	PFUN_GetTaskMap pfnGetTaskMap, 
	PFUN_NotifyUpdate pfnNotifyUpdate, 
	PFUN_IsProcessing pfnIsProcessing, 
	void* thiz
)
{
	CURLM* pCurlM = nullptr;
	CURLMsg* msg = nullptr;
	long L = (0);
	int U = (0);
	int M, Q = -1;
	fd_set R, W, E = { 0 };
	struct timeval T = { 0 };
	int result = EXIT_FAILURE;
	std::map<time_t, CTaskInfoData>* pTaskMap = pfnGetTaskMap(thiz);
	if (pTaskMap && pTaskMap->size() > 0)
	{
		pCurlM = curl_multi_init();

		/* we can optionally limit the total amount of connections this multi handle uses */
		curl_multi_setopt(pCurlM, CURLMOPT_MAXCONNECTS, (long)pTaskMap->size());

		curl_multi_setopt(pCurlM, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);

		for (auto it = pTaskMap->begin(); it != pTaskMap->end(); it++)
		{
			if (!pfnIsProcessing(thiz))
			{
				TASKMAP_CLEANUP(pTaskMap, pCurlM);
				goto __LEAVE_CLEAN__;
			}
			if (it->second.state == TASKSTATETYPE::TSTYPE_NULL)
			{
				it->second.state = TASKSTATETYPE::TSTYPE_WAITING;
			}
			// 如果未开始, 则加入下载队列
			if (it->second.state == TASKSTATETYPE::TSTYPE_WAITING)
			{
				if (U < pTaskMap->size())
				{
					it->second.state = TASKSTATETYPE::TSTYPE_PROCESSING;
					init_task(pCurlM, &it->second);
					U++;
				}
			}
		}

		while (U > 0) {

			if (!pfnIsProcessing(thiz))
			{
				TASKMAP_CLEANUP(pTaskMap, pCurlM);
				goto __LEAVE_CLEAN__;
			}

			//执行并发请求，非阻塞，立即返回
			if (curl_multi_perform(pCurlM, &U) == CURLM_CALL_MULTI_PERFORM)
			{
				if (curl_multi_timeout(pCurlM, &L))
				{
					fprintf(stderr, "E: curl_multi_timeout\n");
					goto __LEAVE_CLEAN__;
				}
				FD_ZERO(&R); FD_ZERO(&W); FD_ZERO(&E);
				//获取cm需要监听的文件描述符集合,如果返回的M等于-1，
				//需要等一会然后再次调用curl_multi_perform,等多久？
				//建议至少60毫秒，但你可能想在你自己的特定条件下测试它，找到一个合适的值
				if (curl_multi_fdset(pCurlM, &R, &W, &E, &M))
				{
					fprintf(stderr, "E: curl_multi_fdset\n");
					goto __LEAVE_CLEAN__;
				}

				if (M == -1) {
					std::this_thread::sleep_for(std::chrono::microseconds(L));
				}
				else
				{
					T.tv_sec = L / 1000;
					T.tv_usec = (L % 1000) * 1000;
					/////////////////////////////////////////////////////////////////
					//确定一个或多个套接口的状态,可查询它的可读性、可写性及错误状态信息。
					//返回值：
					//select()调用返回处于就绪状态并且已经包含在fd_set结构中的描述字总数；
					//如果超时则返回0；否则的话，返回SOCKET_ERROR(-1)错误，
					//应用程序可通过WSAGetLastError()获取相应错误代码。
					if (select(M + 1, &R, &W, &E, &T) == (-1))
					{
						fprintf(stderr, "E: select(%d,%ld): %d: %s\n", M + 1, L, errno, strerror(errno));
						goto __LEAVE_CLEAN__;
					}
				}
				continue;
			}

			//////////////////////////////////////////////////////////////////////////////////////////////////
			//获取当前解析的cURL的相关传输信息
			//查询批处理句柄是否单独的传输线程中有消息或信息返回。
			//消息可能包含诸如从单独的传输线程返回的错误码或者只是传输线程有没有完成之类的报告。
			//重复调用这个函数，它每次都会返回一个新的结果，直到这时没有更多信息返回时，
			//FALSE 被当作一个信号返回。通过msgs_in_queue返回的整数指出将会包含当这次函数被调用后，还剩余的消息数。
			//Warning	返回的资源指向的数据调用curl_multi_remove_handle()后将不会存在。
			while ((msg = curl_multi_info_read(pCurlM, &Q)))
			{
				CTaskInfoData* pTID = nullptr;
				CURLcode curlcode = CURLE_OK;

				curlcode = curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &pTID);

				if (msg->msg == CURLMSG_DONE)
				{
#ifdef CURL_DEBUG
					fprintf(stdout, "R: %d - %s <%s>\n", msg->data.result, curl_easy_strerror(msg->data.result), pTID->url.c_str());
#endif
					pTaskMap->at(pTID->time).state = TASKSTATETYPE::TSTYPE_FINISH;
				}
				else
				{
					fprintf(stderr, "E: CURLMsg (%d)\n", msg->msg);
					pTaskMap->at(pTID->time).state = TASKSTATETYPE::TSTYPE_FAILED;
				}
				if (curlcode == CURLE_OK)
				{
					resp_data.assign(pTID->response_data.data(), pTID->response_data.size());
#ifdef CURL_DEBUG
					fprintf(stdout, "Response: %.*s\n", (int)pTID->response_data.size(), pTID->response_data.data());
#endif
					if (pfnNotifyUpdate)
					{
						pfnNotifyUpdate(thiz);
					}
				}

				curl_slist_free_all(pTID->header_slist);
				pTID->header_slist = nullptr;
				pTaskMap->at(pTID->time).curl = nullptr;
				pTaskMap->erase(pTID->time);

				curl_multi_remove_handle(pCurlM, msg->easy_handle);
				curl_easy_cleanup(msg->easy_handle);
			}
		}

		result = EXIT_SUCCESS;

	__LEAVE_CLEAN__:

		curl_multi_cleanup(pCurlM);
		pCurlM = nullptr;
	}

	return result;
}

class CHttpTask
{
public:
	CHttpTask() :m_nProcessingStatus(0) {};
	~CHttpTask() {};

private:
	std::map<time_t, CTaskInfoData> m_TaskMap;
	int m_nProcessingStatus;
	int m_nLoadingStatus;
	std::thread task_thread;

public:
	std::map<time_t, CTaskInfoData>* GetTaskMap() { return &m_TaskMap; }
	int IsProcessing() { return m_nProcessingStatus; }
	void SetProcessing(int nProcessingStatus) { m_nProcessingStatus = nProcessingStatus; }
public:
	static void NotifyUpdate(void* p)
	{
		CHttpTask* thiz = reinterpret_cast<CHttpTask*>(p);
		if (thiz)
		{
		}
	}
	static int IsProcessing(void* p)
	{
		CHttpTask* thiz = reinterpret_cast<CHttpTask*>(p);
		if (thiz)
		{
			return thiz->IsProcessing();
		}
		return (-1);
	}
	static std::map<time_t, CTaskInfoData>* GetTaskMap(void* p)
	{
		CHttpTask* thiz = reinterpret_cast<CHttpTask*>(p);
		if (thiz)
		{
			return &thiz->m_TaskMap;
		}
		return 0;
	}
	void request(std::string& resp_data)
	{
		CHttpTask* thiz = reinterpret_cast<CHttpTask*>(this);
		if (thiz)
		{
			thiz->SetProcessing(1);
			async_start(resp_data, &CHttpTask::GetTaskMap, &CHttpTask::NotifyUpdate, &CHttpTask::IsProcessing, thiz);
			thiz->SetProcessing(0);
		}
	}
public:
	SINGLETON_INSTANCE(CHttpTask)
};
