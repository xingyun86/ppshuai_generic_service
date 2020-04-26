// libuvdemo.h : Include file for standard system include files,
// or project specific include files.

#pragma once

// TODO: Reference additional headers your program requires here.

#define FUNC_HTTP(N) void N(uWS::HttpResponse<false>* res, uWS::HttpRequest* req)
#define FUNC_HTTPS(N) void N(uWS::HttpResponse<true>* res, uWS::HttpRequest* req)

#define ATTACH_ABORT_HANDLE(res) res->onAborted([]() {std::cout << "ABORTED!" << std::endl;})

#define TIME_START() std::chrono::steady_clock::time_point time_point = std::chrono::steady_clock::now()
#define TIME_CLOSE() std::cout <<"[" << __func__ << "] Cost " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - time_point).count() << "us." << std::endl

class CUwsBus {
public:
	const char* root;
	unsigned short port;
public:
	SINGLETON_INSTANCE(CUwsBus)
};

namespace uWS {

	FUNC_HTTP(test) {
		ATTACH_ABORT_HANDLE(res);
		TIME_START();
		res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("This is a test!");
		TIME_CLOSE();
	}

	FUNC_HTTP(ping) {

		ATTACH_ABORT_HANDLE(res);

		TIME_START();

		/* You can efficiently stream huge files too */
		res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("PONG");

		TIME_CLOSE();
	}
	FUNC_HTTP(hello) {

		ATTACH_ABORT_HANDLE(res);

		TIME_START();

		/* You can efficiently stream huge files too */
		res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("Hello HTTP!");

		TIME_CLOSE();
	}
}
