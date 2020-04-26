// libuvdemo.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include "service.h"
#include <fstream>

// TODO: Reference additional headers your program requires here.
namespace Futures {
	FUNC_HTTP(test) {
		ATTACH_ABORT_HANDLE(res);
		TIME_START(); 
		res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("This is a test!");
		TIME_CLOSE();
	}

	FUNC_HTTP(get_index) {
		ATTACH_ABORT_HANDLE(res);
		TIME_START();

		std::string result = FILE_READER(CUwsBus::GetInstance()->root + std::string(req->getUrl()), std::ios::binary);
		if (result.length() <= 0)
		{
			res->writeHeader("Content-Type", "text/html; charset=utf-8")->end(HTTP_404_NOT_FOUND_TEXT);
		}
		else
		{
			res->writeHeader("Content-Type", "text/html; charset=utf-8")->end(result);
		}

		TIME_CLOSE();
	}
	FUNC_HTTP(get_chart_dce) {
		ATTACH_ABORT_HANDLE(res);
		TIME_START();

		std::string_view query = req->getQuery();
		std::string product_name = "";
		std::string date = "";
		std::string days = "";
		std::string result = "";
		std::vector<std::vector<std::string>> svv;
		
		string_regex_find(result, svv, std::string(query.data(), query.length()), "p=(.*?)&d=(.*?)&c=(.*?)&signature=(.*+)");
		if (svv.size() > 0)
		{
			product_name = svv.at(0).at(0);
			date = svv.at(1).at(0);
			days = svv.at(2).at(0);
			try
			{
				res->writeHeader("Content-Type", "text/html; charset=utf-8")->end(CFutureService::GetInstance()->dce_chart(product_name, date, std::stoi(days)).c_str());
			}
			catch (const std::exception& e)
			{
				std::cout << "Exception:" << e.what() << std::endl;
				res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("param error!");
			}
		}
		else
		{
			res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("param error!");
		}

		TIME_CLOSE();
	}
	FUNC_HTTP(get_chart_dce_list) {
		ATTACH_ABORT_HANDLE(res);
		TIME_START();

		std::string_view query = req->getQuery();
		std::string product_name = "";
		std::string date = "";
		std::string days = "";
		std::string result = "";
		std::vector<std::vector<std::string>> svv;
		string_regex_find(result, svv, std::string(query.data(), query.length()), "p=(.*?)&d=(.*?)&c=(.*?)&signature=(.*+)");
		if (svv.size() > 0)
		{
			product_name = svv.at(0).at(0);
			date = svv.at(1).at(0);
			days = svv.at(2).at(0);
			try
			{
				res->writeHeader("Content-Type", "application/json; charset=utf-8")->end(CFutureService::GetInstance()->dce_chart(product_name, date, std::stoi(days), true).c_str());
			}
			catch (const std::exception& e)
			{
				std::cout << "Exception:" << e.what() << std::endl;
				res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("param error!");
			}
		}
		else
		{
			res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("param error!");
		}

		TIME_CLOSE();
	}
	FUNC_HTTP(get_chart_czce) {
		ATTACH_ABORT_HANDLE(res);
		TIME_START();

		std::string_view query = req->getQuery();
		std::string product_name = "";
		std::string date = "";
		std::string days = "";
		std::string result = "";
		std::vector<std::vector<std::string>> svv;

		string_regex_find(result, svv, std::string(query.data(), query.length()), "p=(.*?)&d=(.*?)&c=(.*?)&signature=(.*+)");
		if (svv.size() > 0)
		{
			product_name = svv.at(0).at(0);
			date = svv.at(1).at(0);
			days = svv.at(2).at(0);
			try
			{
				res->writeHeader("Content-Type", "text/html; charset=utf-8")->end(CFutureService::GetInstance()->czce_chart(product_name, date, std::stoi(days)).c_str());
			}
			catch (const std::exception& e)
			{
				std::cout << "Exception:" << e.what() << std::endl;
				res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("param error!");
			}
		}
		else
		{
			res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("param error!");
		}

		TIME_CLOSE();
	}
	FUNC_HTTP(get_chart_czce_list) {
		ATTACH_ABORT_HANDLE(res);
		TIME_START();

		std::string_view query = req->getQuery();
		std::string product_name = "";
		std::string date = "";
		std::string days = "";
		std::string result = "";
		std::vector<std::vector<std::string>> svv;

		string_regex_find(result, svv, std::string(query.data(), query.length()), "p=(.*?)&d=(.*?)&c=(.*?)&signature=(.*+)");
		if (svv.size() > 0)
		{
			product_name = svv.at(0).at(0);
			date = svv.at(1).at(0);
			days = svv.at(2).at(0);
			try
			{
				res->writeHeader("Content-Type", "application/json; charset=utf-8")->end(CFutureService::GetInstance()->czce_chart(product_name, date, std::stoi(days), true).c_str());
			}
			catch (const std::exception& e)
			{
				std::cout << "Exception:" << e.what() << std::endl;
				res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("param error!");
			}
		}
		else
		{
			res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("param error!");
		}

		TIME_CLOSE();
	}
	FUNC_HTTP(get_chart_shfe) {
		ATTACH_ABORT_HANDLE(res);
		TIME_START();

		std::string_view query = req->getQuery();
		std::string product_name = "";
		std::string date = "";
		std::string days = "";
		std::string result = "";
		std::vector<std::vector<std::string>> svv;

		string_regex_find(result, svv, std::string(query.data(), query.length()), "p=(.*?)&d=(.*?)&c=(.*?)&signature=(.*+)");
		if (svv.size() > 0)
		{
			product_name = svv.at(0).at(0);
			date = svv.at(1).at(0);
			days = svv.at(2).at(0);
			try
			{
				res->writeHeader("Content-Type", "text/html; charset=utf-8")->end(CFutureService::GetInstance()->shfe_chart(product_name, date, std::stoi(days)).c_str());
			}
			catch (const std::exception& e)
			{
				std::cout << "Exception:" << e.what() << std::endl;
				res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("param error!");
			}
		}
		else
		{
			res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("param error!");
		}

		TIME_CLOSE();
	}
	FUNC_HTTP(get_chart_shfe_list) {
		ATTACH_ABORT_HANDLE(res);
		TIME_START();

		std::string_view query = req->getQuery();
		std::string product_name = "";
		std::string date = "";
		std::string days = "";
		std::string result = "";
		std::vector<std::vector<std::string>> svv;

		string_regex_find(result, svv, std::string(query.data(), query.length()), "p=(.*?)&d=(.*?)&c=(.*?)&signature=(.*+)");
		if (svv.size() > 0)
		{
			product_name = svv.at(0).at(0);
			date = svv.at(1).at(0);
			days = svv.at(2).at(0);
			try
			{
				res->writeHeader("Content-Type", "application/json; charset=utf-8")->end(CFutureService::GetInstance()->shfe_chart(product_name, date, std::stoi(days), true).c_str());
			}
			catch (const std::exception& e)
			{
				std::cout << "Exception:" << e.what() << std::endl;
				res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("param error!");
			}
		}
		else
		{
			res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("param error!");
		}

		TIME_CLOSE();
	}
	FUNC_HTTP(get_chart_cffex) {
		ATTACH_ABORT_HANDLE(res);
		TIME_START();

		std::string_view query = req->getQuery();
		std::string product_name = "";
		std::string date = "";
		std::string days = "";
		std::string result = "";
		std::vector<std::vector<std::string>> svv;

		string_regex_find(result, svv, std::string(query.data(), query.length()), "p=(.*?)&d=(.*?)&c=(.*?)&signature=(.*+)");
		if (svv.size() > 0)
		{
			product_name = svv.at(0).at(0);
			date = svv.at(1).at(0);
			days = svv.at(2).at(0);
			try
			{
				res->writeHeader("Content-Type", "text/html; charset=utf-8")->end(CFutureService::GetInstance()->cffex_chart(product_name, date, std::stoi(days)).c_str());
			}
			catch (const std::exception& e)
			{
				std::cout << "Exception:" << e.what() << std::endl;
				res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("param error!");
			}
		}
		else
		{
			res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("param error!");
		}

		TIME_CLOSE();
	}
	FUNC_HTTP(get_chart_cffex_list) {
		ATTACH_ABORT_HANDLE(res);
		TIME_START();

		std::string_view query = req->getQuery();
		std::string product_name = "";
		std::string date = "";
		std::string days = "";
		std::string result = "";
		std::vector<std::vector<std::string>> svv;
		std::chrono::steady_clock::time_point s = std::chrono::steady_clock::now();
		string_regex_find(result, svv, std::string(query.data(), query.length()), "p=(.*?)&d=(.*?)&c=(.*?)&signature=(.*+)");
		if (svv.size() > 0)
		{
			product_name = svv.at(0).at(0);
			date = svv.at(1).at(0);
			days = svv.at(2).at(0);
			try
			{
				res->writeHeader("Content-Type", "application/json; charset=utf-8")->end(CFutureService::GetInstance()->cffex_chart(product_name, date, std::stoi(days), true).c_str());
			}
			catch (const std::exception& e)
			{
				std::cout << "Exception:" << e.what() << std::endl;
				res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("param error!");
			}
		}
		else
		{
			res->writeHeader("Content-Type", "text/html; charset=utf-8")->end("param error!");
		}

		TIME_CLOSE();
	}
}