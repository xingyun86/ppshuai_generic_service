// libuvdemo.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include "service.h"
#include <fstream>

// TODO: Reference additional headers your program requires here.
namespace Github {
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

	FUNC_HTTP(get_size) {
		TIME_START();

		std::string_view query = req->getQuery();
		std::string org = "";
		std::string prj = "";
		std::string result = "";
		std::vector<std::vector<std::string>> svv;
		string_regex_find(result, svv, std::string(query.data(), query.length()), "org=(.*?)&prj=(.*?)&signature=(.*+)");
		if (svv.size() > 0)
		{
			org = svv.at(0).at(0);
			prj = svv.at(1).at(0);
			try
			{
				rapidjson::Document d_resp;
				std::string resp = "";
				std::string url = "https://api.github.com/repos/" + org + "/" + prj;

				bytedata* pbd = bytedata::startup();
				CURLcode curlCode = curl_http_form_execute(pbd, url.c_str());
				if (curlCode == CURLE_OK)
				{
					if (!d_resp.Parse(pbd->p, pbd->s).HasParseError())
					{
						if (d_resp.HasMember("size") && d_resp["size"].IsInt())
						{
							if (d_resp["size"].GetInt() <= 0)
							{
								resp = "{\"error\":\"size <= 0MB\"}";
							}
							else
							{
								//100M
								if ((d_resp["size"].GetInt() + d_resp["size"].GetInt() / 2) >= 102400)
								{
									resp = "{\"error\":\"size >= 400MB\"}";
								}
								else
								{
									system("mkdir -p ./github/downloads/");
									std::string cmd = "wget -q -O ./github/downloads/" + org + "-" + prj + ".zip" + " https://github.com/" + org + "/" + prj + "/archive/master.zip";
									std::cout << "cmd=" << cmd << std::endl;
									std::system(cmd.data());
									resp = "{\"org_prj_size\":" + std::to_string(d_resp["size"].GetInt()) + ",\"url\":\"http://xjp.ppsbbs.tech:8899/github/downloads/" + org + "-" + prj + ".zip" + "\"}";
								}
							}
						}
						else
						{
							resp = "{\"error\":\"no size\"}";
						}
					}
					else
					{
						resp = "{\"error\":\"not json\"}";
					}
				}
				pbd->cleanup();
				res->writeHeader("Content-Type", "application/json; charset=utf-8")->end(resp);
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
	
	FUNC_HTTP(get_downloads) {
		
		ATTACH_ABORT_HANDLE(res);

		TIME_START();

		std::string file_path = CUwsBus::GetInstance()->root + std::string(req->getUrl());
		std::string result = FILE_READER(file_path, std::ios::binary);
		if (result.length() <= 0)
		{
			res->writeHeader("Content-Type", "text/html; charset=utf-8")->end(HTTP_404_NOT_FOUND_TEXT);
		}
		else
		{
			res->writeHeader("Content-Type", "application/zip")->end(result);
		}

		system(("rm -rf " + file_path).data());

		TIME_CLOSE();
	}
}