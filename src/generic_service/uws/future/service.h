// libuvdemo.h : Include file for standard system include files,
// or project specific include files.

#pragma once


// TODO: Reference additional headers your program requires here.
namespace Futures {
#define HANDLE_PRESET()  \
	std::string T = "", X = "", L5 = "", S5 = "", L10 = "", S10 = "", L20 = "", S20 = "", L5S = "", L10S = "", L20S = "";\
	std::map<std::string, std::string> umap;\
	setlocale(LC_ALL, "chs");\
	calc_date_list(umap, date, count);\
	X.append("["); L5.append("["); S5.append("["); L10.append("["); S10.append("[");\
	L20.append("["); S20.append("["); L5S.append("["); L10S.append("["); L20S.append("[");\
	for (auto& it : umap){std::string data("");

#define HANDLE_FINISH(N) \
}if (X.length() > 1) { *X.rbegin() = ']'; } else { X.append("]"); }\
if (L5.length() > 1) { *L5.rbegin() = ']'; }else { L5.append("]"); }\
if (S5.length() > 1) { *S5.rbegin() = ']'; }else { S5.append("]"); }\
if (L10.length() > 1) { *L10.rbegin() = ']'; }else { L10.append("]"); }\
if (S10.length() > 1) { *S10.rbegin() = ']'; }else { S10.append("]"); }\
if (L20.length() > 1) { *L20.rbegin() = ']'; }else { L20.append("]"); }\
if (S20.length() > 1) { *S20.rbegin() = ']'; }else { S20.append("]"); }\
if (L5S.length() > 1) { *L5S.rbegin() = ']'; }else { L5S.append("]"); }\
if (L10S.length() > 1) { *L10S.rbegin() = ']'; }else { L10S.append("]"); }\
if (L20S.length() > 1) { *L20S.rbegin() = ']'; }else { L20S.append("]"); }\
std::string temp=FILE_READER(std::string(CUwsBus::GetInstance()->root) + "/future/chart.html", std::ios::binary);\
string_replace_all(temp, N, "VVVVVV");\
string_replace_all(temp, X, "XXXXXX");\
string_replace_all(temp, L5, "LLL5LLL");\
string_replace_all(temp, S5, "SSS5SSS");\
string_replace_all(temp, L10, "LLL10LLL");\
string_replace_all(temp, S10, "SSS10SSS");\
string_replace_all(temp, L20, "LLL20LLL");\
string_replace_all(temp, S20, "SSS20SSS");\
string_replace_all(temp, L5S, "LLL5SSS");\
string_replace_all(temp, L10S, "LLL10SSS");\
string_replace_all(temp, L20S, "LLL20SSS");\
return temp;

	class CFutureService{
	public:
		CFutureService() {}
		virtual ~CFutureService() {}
	private:

		bool is_trade_day(const time_t& tt)
		{
			struct tm* ptm = localtime(&tt);
			if ((ptm->tm_wday >= 1) && (ptm->tm_wday <= 5))
			{
				return true;
			}
			return false;
		}
		time_t get_last_trade_day(const time_t& tt)
		{
			time_t last_tt = tt - 86400;
			struct tm* ptm = localtime(&tt);
			while (!is_trade_day(last_tt))
			{
				last_tt -= 86400;
			}
			return last_tt;
		}
		std::string time_t_to_date(const time_t& tt)
		{
			struct tm* ptm = localtime(&tt);
			std::string date(16, '\0');
			snprintf((char *)date.data(), date.size(), "%04d%02d%02d\0", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
			return date.c_str();
		}
		void calc_date_list(std::map<std::string, std::string>& umap, const std::string& date, int count)
		{
			struct tm tm = { 0 };
			tm.tm_year = std::stoi(date.substr(0, 4)) - 1900;
			tm.tm_mon = std::stoi(date.substr(4, 2)) - 1;
			tm.tm_mday = std::stoi(date.substr(6, 2));
			time_t date_tt = mktime(&tm);
			for (size_t i = 0; i < count; i++)
			{
				if (!is_trade_day(date_tt))
				{
					date_tt = get_last_trade_day(date_tt);
				}
				umap.insert(std::map<std::string, std::string>::value_type(time_t_to_date(date_tt), ""));
				date_tt = get_last_trade_day(date_tt);
			}
		}
	public:
		std::string dce_chart(const std::string& product_name, const std::string& date, int count, bool bGetCode = false)
		{
			HANDLE_PRESET()
			data = FILE_READER("/usr/share/nginx/html/foot-wash/storage/app/images/edc/" + it.first + "/dce.csv", std::ios::binary);
			if (data.length() > 0)
			{
				bool flag = false;
				std::string result("");
				std::string pattern1 = "合约代码：(.*?),Date：(.*?),\r\n";
				std::string pattern2 = "总计,(.*?),(.*?),\r\n";
				std::string pattern3 = "(.*?),(.*?),(.*?),(.*?),\r\n";
				std::vector<std::vector<std::string>> svv1;
				std::vector<std::vector<std::string>> svv2;
				std::vector<std::vector<std::string>> svv3;
				std::string out(data.size() * 4, '\0');
				size_t in_len = data.size();
				size_t out_len = out.size();
				flag = gb2312_to_utf8((char*)data.c_str(), &in_len, (char*)out.c_str(), &out_len);
				flag = string_regex_find(result, svv1, out.c_str(), pattern1);
				int nIndex1 = (-1);
				if (svv1.size())
				{
					if (bGetCode)
					{
						std::string code_list("[");
						for (size_t i = 0; i < svv1.at(0).size(); i++)
						{
							if (i > 0)
							{
								code_list.append(",");
							}
							code_list.append("\"").append(svv1.at(0).at(i)).append("\"");
						}
						code_list.append("]");
						return code_list;
					}
					for (size_t i = 0; i < svv1.at(0).size(); i++)
					{
						if (svv1.at(0).at(i).compare(product_name) == 0)
						{
							nIndex1 = i;
							break;
						}
					}
					if (nIndex1 >= 0 && nIndex1 < svv1.at(0).size())
					{
						// 2-多单手数
						// 3-变化手数
						int nChangeIndex = 1;
						T = svv1.at(0).at(nIndex1).c_str();
						X.append("'").append(it.first).append("',");

						flag = string_regex_find(result, svv3, out.c_str(), pattern3);
						int nIndexAll = 0;
						int nIndexLong = 0;
						int nIndexShort = 0;
						int nIndexCounter = 0;
						for (size_t i = 0; i < svv3.begin()->size(); i++)
						{
							if (svv3.at(0).at(i).compare("名次") == 0)
							{
								if ((nIndex1 * 3) == nIndexCounter)
								{
									nIndexAll = i;
									for (size_t j = nIndexAll + 1; j < svv3.begin()->size(); j++)
									{
										if (nIndexLong == 0 && nIndexShort == 0 && svv3.at(0).at(j).compare("名次") == 0)
										{
											nIndexLong = j;
										}
										else if (nIndexLong > 0 && nIndexShort == 0 && svv3.at(0).at(j).compare("名次") == 0)
										{
											nIndexShort = j;
											break;
										}
									}
									break;
								}
								nIndexCounter++;
							}
						}
						if (svv3.size() && nIndexLong > 0 && nIndexShort > 0)
						{
							int nSumLong = 0;
							int nSumShort = 0;
							for (size_t i = 1; i <= 5; i++)
							{
								if (svv3.at(0).at(nIndexLong + i).compare("名次") == 0)
								{
									break;
								}
								nSumLong += std::stoi(svv3.at(2).at(nIndexLong + i).c_str());
							}
							for (size_t i = 1; i <= 5; i++)
							{
								if (svv3.at(0).at(nIndexShort + i).compare("名次") == 0)
								{
									break;
								}
								nSumShort += std::stoi(svv3.at(2).at(nIndexShort + i).c_str());
							}
							//持买量
							S5.append("'").append(std::to_string(nSumShort)).append("',");
							//持卖量
							L5.append("'").append(std::to_string(nSumLong)).append("',");
							//持买量-持卖量
							L5S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
							for (size_t i = 6; i <= 10; i++)
							{
								if (svv3.at(0).at(nIndexLong + i).compare("名次") == 0)
								{
									break;
								}
								nSumLong += std::stoi(svv3.at(2).at(nIndexLong + i).c_str());
							}
							for (size_t i = 6; i <= 10; i++)
							{
								if (svv3.at(0).at(nIndexShort + i).compare("名次") == 0)
								{
									break;
								}
								nSumShort += std::stoi(svv3.at(2).at(nIndexShort + i).c_str());
							}
							//持买量
							L10.append("'").append(std::to_string(nSumLong)).append("',");
							//持卖量
							S10.append("'").append(std::to_string(nSumShort)).append("',");
							//持买量-持卖量
							L10S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
							for (size_t i = 11; i <= 20; i++)
							{
								if (svv3.at(0).at(nIndexLong + i).compare("名次") == 0)
								{
									break;
								}
								nSumLong += std::stoi(svv3.at(2).at(nIndexLong + i).c_str());
							}
							for (size_t i = 11; i <= 20; i++)
							{
								if (svv3.at(0).at(nIndexShort + i).compare("名次") == 0)
								{
									break;
								}
								nSumShort += std::stoi(svv3.at(2).at(nIndexShort + i).c_str());
							}
							//持买量
							L20.append("'").append(std::to_string(nSumLong)).append("',");
							//持卖量
							S20.append("'").append(std::to_string(nSumShort)).append("',");
							//持买量-持卖量
							L20S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
						}
					}
				}
			}
			HANDLE_FINISH("DCE")
		}
		std::string czce_chart(const std::string& product_name, const std::string& date, int count, bool bGetCode = false)
		{
			HANDLE_PRESET()
			data = FILE_READER("/usr/share/nginx/html/foot-wash/storage/app/images/edc/" + it.first + "/czce.csv", std::ios::binary);
			if (data.length() > 0)
			{
				bool flag = false;
				std::string result("");
				std::string pattern1 = "\"(.*?)\",\"(.*?)\",\"(.*?)\",\"(.*?)\",\"(.*?)\",\"(.*?)\",\"(.*?)\",\"(.*?)\",\"(.*?)\",\"(.*?)\"\r\n";
				std::vector<std::vector<std::string>> svv1;
				std::string out(data.size() * 4, '\0');
				size_t in_len = data.size();
				size_t out_len = out.size();
				flag = gb2312_to_utf8((char*)data.c_str(), &in_len, (char*)out.c_str(), &out_len);
				string_replace_all(out, "", " ");
				if (bGetCode)
				{
					std::string pattern = "品种：(.*?)日期";
					flag = string_regex_find(result, svv1, out.c_str(), pattern);
					std::string code_list("[");
					if (svv1.size())
					{
						for (size_t i = 0; i < svv1.at(0).size(); i++)
						{
							if (code_list.length() > 1)
							{
								code_list.append(",");
							}
							code_list.append("\"").append(svv1.at(0).at(i)).append("\"");
						}
					}
					pattern = "合约：(.*?)日期";
					flag = string_regex_find(result, svv1, out.c_str(), pattern);
					if (svv1.size())
					{
						for (size_t i = 0; i < svv1.at(0).size(); i++)
						{
							if (code_list.length() > 1)
							{
								code_list.append(",");
							}
							code_list.append("\"").append(svv1.at(0).at(i)).append("\"");
						}
					}
					code_list.append("]");
					return code_list;
				}
				flag = string_regex_find(result, svv1, out.c_str(), pattern1);
				int nIndex1 = (-1);
				if (svv1.size())
				{
					for (size_t i = 0; i < svv1.at(0).size(); i++)
					{
						if (svv1.at(0).at(i).find(product_name) != std::string::npos)
						{
							nIndex1 = i;
							break;
						}
					}
					if (nIndex1 >= 0 && nIndex1 < svv1.at(0).size())
					{
						// 2-多单手数
						// 3-变化手数
						int nChangeIndex = 1;
						T = svv1.at(0).at(nIndex1).c_str();
						X.append("'").append(it.first).append("',");

						int nSumLong = 0;
						int nSumShort = 0;
						nIndex1 = nIndex1 + 1;
						for (size_t i = nIndex1 + 1; i < svv1.at(0).size(); i++)
						{
							if (svv1.at(0).at(i).compare(0, strlen("合计"), "合计") == 0)
							{
								if (nIndex1 + 5 > i)
								{
									//持买量
									S5.append("'").append(std::to_string(nSumShort)).append("',");
									//持卖量
									L5.append("'").append(std::to_string(nSumLong)).append("',");
									//持买量-持卖量
									L5S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
								}
								else if (nIndex1 + 10 > i)
								{
									//持买量
									S10.append("'").append(std::to_string(nSumShort)).append("',");
									//持卖量
									L10.append("'").append(std::to_string(nSumLong)).append("',");
									//持买量-持卖量
									L10S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
								}
								else if (nIndex1 + 20 > i)
								{
									//持买量
									S20.append("'").append(std::to_string(nSumShort)).append("',");
									//持卖量
									L20.append("'").append(std::to_string(nSumLong)).append("',");
									//持买量-持卖量
									L20S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
								}
								break;
							}
							std::string sumLong = svv1.at(5).at(i);
							string_replace_all(sumLong, "", ",");
							string_replace_all(sumLong, "", " ");
							std::string sumShort = svv1.at(8).at(i);
							string_replace_all(sumShort, "", ",");
							string_replace_all(sumShort, "", " ");
							nSumLong += std::stoi(sumLong);
							nSumShort += std::stoi(sumShort);
							if (nIndex1 + 5 == i)
							{
								//持买量
								S5.append("'").append(std::to_string(nSumShort)).append("',");
								//持卖量
								L5.append("'").append(std::to_string(nSumLong)).append("',");
								//持买量-持卖量
								L5S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
							}
							else if (nIndex1 + 10 == i)
							{
								//持买量
								S10.append("'").append(std::to_string(nSumShort)).append("',");
								//持卖量
								L10.append("'").append(std::to_string(nSumLong)).append("',");
								//持买量-持卖量
								L10S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
							}
							else if (nIndex1 + 20 == i)
							{
								//持买量
								S20.append("'").append(std::to_string(nSumShort)).append("',");
								//持卖量
								L20.append("'").append(std::to_string(nSumLong)).append("',");
								//持买量-持卖量
								L20S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
							}
						}
					}
				}
			}

			HANDLE_FINISH("CZCE")
		}

		std::string shfe_chart(const std::string& product_name, const std::string& date, int count, bool bGetCode = false)
		{
			HANDLE_PRESET()
			data = FILE_READER("/usr/share/nginx/html/foot-wash/storage/app/images/edc/" + it.first + "/shfe.csv", std::ios::binary);
			if (data.length() > 0)
			{
				bool flag = false;
				std::string result("");
				std::string pattern1 = "(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?)\r\n";
				std::vector<std::vector<std::string>> svv1;
				std::string out(data.size() * 4, '\0');
				size_t in_len = data.size();
				size_t out_len = out.size();
				flag = gb2312_to_utf8((char*)data.c_str(), &in_len, (char*)out.c_str(), &out_len);
				string_replace_all(out, "", " ");
				if (bGetCode)
				{
					std::string pattern = "合计:(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?)\r\n";
					flag = string_regex_find(result, svv1, out.c_str(), pattern);
					std::string code_list("[");
					for (size_t i = 0; i < svv1.at(0).size(); i++)
					{
						if (i > 0)
						{
							code_list.append(",");
						}
						code_list.append("\"").append(svv1.at(0).at(i)).append("\"");
					}
					code_list.append("]");
					return code_list;
				}
				flag = string_regex_find(result, svv1, out.c_str(), pattern1);
				int nIndex1 = (-1);
				if (svv1.size())
				{
					for (size_t i = 0; i < svv1.at(0).size(); i++)
					{
						if (svv1.at(0).at(i).find(product_name) != std::string::npos)
						{
							nIndex1 = i;
							break;
						}
					}
					if (nIndex1 >= 0 && nIndex1 < svv1.at(0).size())
					{
						// 2-多单手数
						// 3-变化手数
						int nChangeIndex = 1;
						T = svv1.at(0).at(nIndex1).c_str();
						X.append("'").append(it.first).append("',");

						int nSumLong = 0;
						int nSumShort = 0;
						for (size_t i = nIndex1 + 1; i < svv1.at(0).size(); i++)
						{
							if (svv1.at(0).at(i).compare(0, strlen("合计"), "合计") == 0)
							{
								if (nIndex1 + 5 > i)
								{
									//持买量
									S5.append("'").append(std::to_string(nSumShort)).append("',");
									//持卖量
									L5.append("'").append(std::to_string(nSumLong)).append("',");
									//持买量-持卖量
									L5S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
								}
								else if (nIndex1 + 10 > i)
								{
									//持买量
									S10.append("'").append(std::to_string(nSumShort)).append("',");
									//持卖量
									L10.append("'").append(std::to_string(nSumLong)).append("',");
									//持买量-持卖量
									L10S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
								}
								else if (nIndex1 + 20 > i)
								{
									//持买量
									S20.append("'").append(std::to_string(nSumShort)).append("',");
									//持卖量
									L20.append("'").append(std::to_string(nSumLong)).append("',");
									//持买量-持卖量
									L20S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
								}
								break;
							}
							nSumLong += std::stoi(svv1.at(7).at(i).c_str());
							nSumShort += std::stoi(svv1.at(11).at(i).c_str());
							if (nIndex1 + 5 == i)
							{
								//持买量
								S5.append("'").append(std::to_string(nSumShort)).append("',");
								//持卖量
								L5.append("'").append(std::to_string(nSumLong)).append("',");
								//持买量-持卖量
								L5S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
							}
							else if (nIndex1 + 10 == i)
							{
								//持买量
								S10.append("'").append(std::to_string(nSumShort)).append("',");
								//持卖量
								L10.append("'").append(std::to_string(nSumLong)).append("',");
								//持买量-持卖量
								L10S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
							}
							else if (nIndex1 + 20 == i)
							{
								//持买量
								S20.append("'").append(std::to_string(nSumShort)).append("',");
								//持卖量
								L20.append("'").append(std::to_string(nSumLong)).append("',");
								//持买量-持卖量
								L20S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
							}
						}
					}
				}
			}

			HANDLE_FINISH("SHFE")
		}

		std::string cffex_chart(const std::string& product_name, const std::string& date, int count, bool bGetCode = false)
		{
			HANDLE_PRESET()
			data = FILE_READER("/usr/share/nginx/html/foot-wash/storage/app/images/edc/" + it.first + "/cffex.csv", std::ios::binary);
			if (data.length() > 0)
			{
				bool flag = false;
				std::string result("");
				std::string pattern1 = "(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?)\n";
				std::vector<std::vector<std::string>> svv1;
				std::string out(data.size() * 4, '\0');
				size_t in_len = data.size();
				size_t out_len = out.size();
				flag = gb2312_to_utf8((char*)data.c_str(), &in_len, (char*)out.c_str(), &out_len);
				string_replace_all(out, "", " ");
				if (bGetCode)
				{
					std::string pattern = ",(.*?),期货公司";
					flag = string_regex_find(result, svv1, out.c_str(), pattern);
					std::string code_list("[");
					if (svv1.size())
					{
						for (size_t i = 0; i < svv1.at(0).size(); i++)
						{
							if (code_list.length() > 1)
							{
								code_list.append(",");
							}
							code_list.append("\"").append(svv1.at(0).at(i)).append("\"");
						}
					}
					else
					{
						std::string pattern = ",(.*?),1,(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?),(.*?)\n";
						flag = string_regex_find(result, svv1, out.c_str(), pattern);
						if (svv1.size())
						{
							for (size_t i = 0; i < svv1.at(0).size(); i++)
							{
								if (code_list.length() > 1)
								{
									code_list.append(",");
								}
								code_list.append("\"").append(svv1.at(0).at(i)).append("\"");
							}
						}
					}
					code_list.append("]");
					return code_list;
				}
				flag = string_regex_find(result, svv1, out.c_str(), pattern1);
				int nIndex1 = (-1);
				if (svv1.size())
				{
					for (size_t i = 0; i < svv1.at(0).size(); i++)
					{
						if (svv1.at(0).at(i).compare(it.first) == 0 &&
							svv1.at(2).at(i).at(0) >= '1' &&
							svv1.at(2).at(i).at(0) <= '9' &&
							svv1.at(1).at(i).find(product_name) != std::string::npos)
						{
							nIndex1 = i;
							break;
						}
					}
					if (nIndex1 >= 0 && nIndex1 < svv1.at(0).size())
					{
						// 2-多单手数
						// 3-变化手数
						int nChangeIndex = 1;
						T = svv1.at(0).at(nIndex1).c_str();
						X.append("'").append(it.first).append("',");

						int nSumLong = 0;
						int nSumShort = 0;
						nIndex1 = nIndex1 - 1;
						for (size_t i = nIndex1 + 1; i < svv1.at(0).size(); i++)
						{
							if (svv1.at(1).at(i).compare(0, strlen(product_name.c_str()), product_name.c_str()) != 0)
							{
								if (nIndex1 + 5 > i)
								{
									//持买量
									S5.append("'").append(std::to_string(nSumShort)).append("',");
									//持卖量
									L5.append("'").append(std::to_string(nSumLong)).append("',");
									//持买量-持卖量
									L5S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
								}
								else if (nIndex1 + 10 > i)
								{
									//持买量
									S10.append("'").append(std::to_string(nSumShort)).append("',");
									//持卖量
									L10.append("'").append(std::to_string(nSumLong)).append("',");
									//持买量-持卖量
									L10S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
								}
								else if (nIndex1 + 20 > i)
								{
									//持买量
									S20.append("'").append(std::to_string(nSumShort)).append("',");
									//持卖量
									L20.append("'").append(std::to_string(nSumLong)).append("',");
									//持买量-持卖量
									L20S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
								}
								break;
							}
							std::string sumLong = svv1.at(7).at(i);
							string_replace_all(sumLong, "", ",");
							string_replace_all(sumLong, "", " ");
							std::string sumShort = svv1.at(10).at(i);
							string_replace_all(sumShort, "", ",");
							string_replace_all(sumShort, "", " ");
							nSumLong += std::stoi(sumLong);
							nSumShort += std::stoi(sumShort);
							if (nIndex1 + 5 == i)
							{
								//持买量
								S5.append("'").append(std::to_string(nSumShort)).append("',");
								//持卖量
								L5.append("'").append(std::to_string(nSumLong)).append("',");
								//持买量-持卖量
								L5S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
							}
							else if (nIndex1 + 10 == i)
							{
								//持买量
								S10.append("'").append(std::to_string(nSumShort)).append("',");
								//持卖量
								L10.append("'").append(std::to_string(nSumLong)).append("',");
								//持买量-持卖量
								L10S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
							}
							else if (nIndex1 + 20 == i)
							{
								//持买量
								S20.append("'").append(std::to_string(nSumShort)).append("',");
								//持卖量
								L20.append("'").append(std::to_string(nSumLong)).append("',");
								//持买量-持卖量
								L20S.append("'").append(std::to_string(nSumLong - nSumShort)).append("',");
							}
						}
					}
				}
			}
			HANDLE_FINISH("CFFEX")
		}
	public:
		SINGLETON_INSTANCE(CFutureService)
	};
}