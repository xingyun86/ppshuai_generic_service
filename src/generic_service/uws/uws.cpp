/* This is a simple HTTP(S) web server much like Python's SimpleHTTPServer */

#include "../utils/utils.h"

#include <App.h>
#include "uws.h"

/* Helpers for this example */
#include "helpers/AsyncFileReader.h"
#include "helpers/AsyncFileStreamer.h"
#include "helpers/Middleware.h"

/* optparse */
#define OPTPARSE_IMPLEMENTATION
#include "helpers/optparse.h"

#include "future/interface.h"
#include "github/interface.h"

int main(int argc, char** argv)
{
	//std::string data = Futures::CFutureService::GetInstance()->cffex_chart("", "20200316", 1, true);
	//return 0;
	/*{
		std::unordered_map<std::string, std::string> file_list;
		enum_file(file_list, "d:\\");
		return 0;
	}*/
	/*{
		bytedata* pbd = bytedata::startup();
		CURLcode curlCode = CURLE_OK;
		curlCode = curl_http_startup();
		curlCode = curl_http_form_execute(pbd, "http://chromedriver.storage.googleapis.com/index.html");
		if (curlCode == CURLE_OK)
		{
			printf("ret=%d\n", pbd->s);
			file_writer(std::string(pbd->p, pbd->s), "test.html");
		}
		curl_http_cleanup();
		pbd->cleanup();
	}*/
	//test_chart();
	//return 0;
	int option;
	struct optparse options;
	optparse_init(&options, argv);
	/* ws->getUserData returns one of these */
	struct PerSocketData {

	};
	struct optparse_long longopts[] = {
		{"port", 'p', OPTPARSE_REQUIRED},
		{"help", 'h', OPTPARSE_NONE},
		{"passphrase", 'a', OPTPARSE_REQUIRED},
		{"key", 'k', OPTPARSE_REQUIRED},
		{"cert", 'c', OPTPARSE_REQUIRED},
		{"dh_params", 'd', OPTPARSE_REQUIRED},
		{0}
	};

	struct us_socket_context_options_t ssl_options = {};
	/* Either serve over HTTP or HTTPS */
	struct us_socket_context_options_t empty_ssl_options = {};

	CUwsBus::GetInstance()->port = 3000;

	while ((option = optparse_long(&options, longopts, nullptr)) != -1) {
		switch (option) {
		case 'p':
			CUwsBus::GetInstance()->port = atoi(options.optarg);
			break;
		case 'a':
			ssl_options.passphrase = options.optarg;
			break;
		case 'c':
			ssl_options.cert_file_name = options.optarg;
			break;
		case 'k':
			ssl_options.key_file_name = options.optarg;
			break;
		case 'd':
			ssl_options.dh_params_file_name = options.optarg;
			break;
		case 'h':
		case '?':
		fail:
			std::cout << "Usage: " << argv[0] << " [--help] [--port <port>] [--key <ssl key>] [--cert <ssl cert>] [--passphrase <ssl key passphrase>] [--dh_params <ssl dh params file>] <public root>" << std::endl;
			return 0;
		}
	}

	CUwsBus::GetInstance()->root = optparse_arg(&options);
	if (!CUwsBus::GetInstance()->root) {
		goto fail;
	}
	CURLcode curlCode = CURL_HTTP_STARTUP();
	/* Simple echo websocket server, using multiple threads */
	std::vector<std::shared_ptr<std::thread>> threads(std::thread::hardware_concurrency());
	
	std::transform(threads.begin(), threads.end(), threads.begin(), [&](std::shared_ptr<std::thread> t) {
		
		return std::make_shared<std::thread>([&]() {
			AsyncFileStreamer asyncFileStreamer(CUwsBus::GetInstance()->root);
			if (memcmp(&ssl_options, &empty_ssl_options, sizeof(empty_ssl_options))) {
				/* HTTPS */
				uWS::SSLApp(ssl_options).ws<PerSocketData>("/*", {
						// Settings
						.compression = uWS::SHARED_COMPRESSOR,
						.maxPayloadLength = 16 * 1024,
						.idleTimeout = 10,
						.maxBackpressure = 1 * 1024 * 1024,
						// Handlers
						.open = [](auto* ws, auto* req) {
							std::cout << "open" << std::endl;
						},
						.message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
							std::cout << "message" << std::endl;
							ws->send(message, opCode);
						},
						.drain = [](auto* ws) {
							// Check getBufferedAmount here
							std::cout << "drain" << std::endl;
						},
						.ping = [](auto* ws) {
							std::cout << "ping" << std::endl;
						},
						.pong = [](auto* ws) {
							std::cout << "pong" << std::endl;
						},
						.close = [](auto* ws, int code, std::string_view message) {
							std::cout << "Close:" << code << std::endl;
						}
					})
					.get("/*", [&asyncFileStreamer](auto* res, auto* req) {
						ATTACH_ABORT_HANDLE(res);
						serveFile(res, req);
						asyncFileStreamer.streamFile(res, req->getUrl());
					})
					.post("/*", [](auto* res, auto* req) {
						ATTACH_ABORT_HANDLE(res);
						std::string_view url = req->getUrl();
						std::string_view query = req->getQuery();
						std::string_view param0 = req->getParameter(0);
						std::string_view param1 = req->getParameter(1);

						/* Allocate automatic, stack, variable as usual */
						std::string buffer("");
						/* Move it to storage of lambda */
						res->onData([res, buffer = std::move(buffer)](std::string_view data, bool last) mutable {
							/* Mutate the captured data */
							buffer.append(data.data(), data.length());

							if (last) {
								/* Use the data */
								std::cout << "We got all data, length: " << buffer.length() << std::endl;
								std::cout << "data=" << buffer << std::endl;
								//us_listen_socket_close(listen_socket);
								//res->end("Thanks for the data!");
								{
									std::string data(buffer.c_str(), buffer.length());
									FILE_WRITER(data, "out.txt", std::ios::binary);
									std::string value = string_reader(data, "----------------------------", "\r\n");
									std::string flags = "----------------------------" + value;
									std::cout << "#######################################################" << std::endl;
									std::cout << "value=" << value << std::endl;

									//rapidjson::Document d;
									//rapidjson::Value& v = body_to_json(d, data, flags + "\r\n");
									//printf("value = %s\n", JSON_VALUE_2_STRING(v).c_str());
								}
								res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("[OK]");
								std::cout << "end req" << std::endl;
								/* When this socket dies (times out) it will RAII release everything */
							}
						});
						/* Unwind stack, delete buffer, will just skip (heap) destruction since it was moved */
							})
					.listen(CUwsBus::GetInstance()->port, [](auto* token) {
						if (token) {
							std::cout << "Serving " << CUwsBus::GetInstance()->root << " over HTTPS a " << CUwsBus::GetInstance()->port << std::endl;
							std::cout << "Thread " << std::this_thread::get_id() << " listening on port " << CUwsBus::GetInstance()->port << std::endl;
						}
						else {
							std::cout << "Thread " << std::this_thread::get_id() << " failed to listen on port " << CUwsBus::GetInstance()->port << std::endl;
						}
					}).run();
			}
			else {
				/* HTTP */
				uWS::App()
					.ws<PerSocketData>("/*", {
						// Settings
						.compression = uWS::SHARED_COMPRESSOR,
						.maxPayloadLength = 16 * 1024,
						.idleTimeout = 10,
						.maxBackpressure = 1 * 1024 * 1024,
						// Handlers
						.open = [](auto* ws, auto* req) {
							std::cout << "open" << std::endl;
						},
						.message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
							std::cout << "message" << std::endl;
							ws->send(message, opCode);
						},
						.drain = [](auto* ws) {
							// Check getBufferedAmount here
							std::cout << "drain" << std::endl;
						},
						.ping = [](auto* ws) {
							std::cout << "ping" << std::endl;
						},
						.pong = [](auto* ws) {
							std::cout << "pong" << std::endl;
						},
						.close = [](auto* ws, int code, std::string_view message) {
							std::cout << "Close:" << code << std::endl;
						}
					})
					.get("/test", uWS::test)
					.get("/ping", uWS::ping)
					.get("/hello", uWS::hello)
					.get("/future/index.html", Futures::get_index)
					.get("/future/chart-dce", Futures::get_chart_dce)
					.get("/future/chart-dce-list", Futures::get_chart_dce_list)
					.get("/future/chart-czce", Futures::get_chart_czce)
					.get("/future/chart-czce-list", Futures::get_chart_czce_list)
					.get("/future/chart-shfe", Futures::get_chart_shfe)
					.get("/future/chart-shfe-list", Futures::get_chart_shfe_list)
					.get("/future/chart-cffex", Futures::get_chart_cffex)
					.get("/future/chart-cffex-list", Futures::get_chart_cffex_list)
					.get("/github/index.html", Github::get_index)
					.get("/github/size", Github::get_size)
					.get("/github/downloads/*", Github::get_downloads)
					.get("/*", [&asyncFileStreamer](auto* res, auto* req) {
							ATTACH_ABORT_HANDLE(res);serveFile(res, req);asyncFileStreamer.streamFile(res, req->getUrl());
						})
					.post("/*", [](auto* res, auto* req) {
						ATTACH_ABORT_HANDLE(res);
						std::string_view url = req->getUrl();
						std::string_view query = req->getQuery();
						std::string_view param0 = req->getParameter(0);
						std::string_view param1 = req->getParameter(1);
						std::cout << url << "," << query << "," << param0 << "," << param1 << std::endl;

						/* Allocate automatic, stack, variable as usual */
						std::string buffer = "";
						/* Move it to storage of lambda */
						res->onData([res, buffer = std::move(buffer)](std::string_view data, bool last) mutable {
							/* Mutate the captured data */
							buffer.append(data.data(), data.length());

							if (last) {
								/* Use the data */
								std::cout << "We got all data, length: " << buffer.length() << std::endl;
								std::cout << "data=" << buffer << std::endl;
								//us_listen_socket_close(listen_socket);
								//res->end("Thanks for the data!");
								{
									std::string data(buffer.c_str(), buffer.length());
									//FILE_WRITER(data, "out.txt");
									std::string value = string_reader(data, "----------------------------", "\r\n");
									std::string flags = "----------------------------" + value;
									std::cout << "#######################################################" << std::endl;
									std::cout << "value=" << value << std::endl;

									rapidjson::Document d;
									rapidjson::Value& v = body_to_json(d, data, flags + "\r\n");
									printf("value = %s\n", JSON_VALUE_2_STRING(v).c_str());
								}
								res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("[OK]");
								std::cout << "end req" << std::endl;
								/* When this socket dies (times out) it will RAII release everything */
							}
						});
						/* Unwind stack, delete buffer, will just skip (heap) destruction since it was moved */
					})
					.listen(CUwsBus::GetInstance()->port, [](auto* token) {
						if (token) {
							std::cout << "Serving " << CUwsBus::GetInstance()->root << " over HTTP a " << CUwsBus::GetInstance()->port << std::endl;
							std::cout << "Thread " << std::this_thread::get_id() << " listening on port " << CUwsBus::GetInstance()->port << std::endl;
						}
						else {
							std::cout << "Thread " << std::this_thread::get_id() << " failed to listen on port " << CUwsBus::GetInstance()->port << std::endl;
						}
					}).run();
			}
			});
		});
	std::for_each(threads.begin(), threads.end(), [](std::shared_ptr<std::thread> t) {
		t->join();
		});
	CURL_HTTP_CLEANUP();
}
