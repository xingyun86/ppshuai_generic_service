#include <cstdio>

#include <service_modules.h>

int main(int argc, char** argv)
{
	std::cout << "hello from huobitrade!" << std::endl;
	std::string resp = "";
	std::string spot_account_id = "";
	std::string spot_account_type = "spot";
	std::string spot_account_state = "working";
	CServiceModules::GetInstance()->service_startup();
	CServiceModules::GetInstance()->get_system_state(resp);
	CServiceModules::GetInstance()->get_symbols_info(resp);
	CServiceModules::GetInstance()->get_currencys_info(resp);
	CServiceModules::GetInstance()->get_timestamp_info(resp);
	CServiceModules::GetInstance()->get_accounts_info(resp);
	rapidjson::Document d_accounts; rapidjson::Value& v_accounts = STRING_2_JSON_VALUE(d_accounts, resp);
	if (!v_accounts.ObjectEmpty())
	{
		if (CHECK_JSON_STR(v_accounts, "status"))
		{
			if (std::string(v_accounts["status"].GetString()).compare("ok") == 0)
			{
				if (CHECK_JSON_ARY(v_accounts, "data"))
				{
					for (rapidjson::SizeType i = 0; i < v_accounts["data"].Size(); i++)
					{
						rapidjson::Value& v_account = v_accounts["data"][i];
						if (CHECK_JSON_STR(v_account, "type")
							&& CHECK_JSON_INT(v_account, "id")
							&& CHECK_JSON_STR(v_account, "subtype")
							&& CHECK_JSON_STR(v_account, "state"))
						{
							if (spot_account_state.compare(v_account["state"].GetString()) == 0)
							{
								if (spot_account_type.compare(v_account["type"].GetString()) == 0)
								{
									spot_account_id = std::to_string(v_account["id"].GetInt());
								}
								//CServiceModules::GetInstance()->get_account_balance(resp, std::to_string(v_account["id"].GetInt()));
								//CServiceModules::GetInstance()->get_account_history(resp, {
								//	{"account-id", std::to_string(v_account["id"].GetInt())},
								//	{"currency", "usdt"},
								//	{"start-time", std::to_string(std::time(nullptr) - 30 * 24 * 60 * 60)},
								//	{"end-time", std::to_string(std::time(nullptr))}, }
								//);
							}
						}
					}
				}
			}
		}
	}

	/*CServiceModules::GetInstance()->get_order_history(resp, {
		{"symbol", "bsvusdt"},
		{"states", "filled"},
		}
	);*/
	CServiceModules::GetInstance()->get_order_history(resp, {
		{"symbol", "ethusdt"},
		{"states", "filled"},
		}
	);
	CServiceModules::GetInstance()->get_order_history(resp, {
		{"symbol", "zecusdt"},
		{"states", "filled"},
		}
	);
	CServiceModules::GetInstance()->get_openorders_list(resp, {
		{"account-id", spot_account_id},
		{"symbol", "ethusdt"},
		}
	);
	CServiceModules::GetInstance()->get_openorders_list(resp, {
		{"account-id", spot_account_id},
		{"symbol", "zecusdt"},
		}
	);
	std::map<std::string, CMarketTicker> symbols_tickers = {
		{"zecusdt",{}},
		{"ethusdt",{}},
	};
	while (true)
	{
		CServiceModules::GetInstance()->get_market_tickers(resp);
		rapidjson::Document d_market_tickers;
		rapidjson::Value& v_market_tickers = STRING_2_JSON_VALUE(d_market_tickers, resp);
		if (!v_market_tickers.ObjectEmpty())
		{
			if (CHECK_JSON_STR(v_market_tickers, "status"))
			{
				if (std::string(v_market_tickers["status"].GetString()).compare("ok") == 0)
				{
					if (CHECK_JSON_ARY(v_market_tickers, "data"))
					{
						for (rapidjson::SizeType i = 0; i < v_market_tickers["data"].Size(); i++)
						{
							rapidjson::Value& v_market_ticker = v_market_tickers["data"][i];
							if (symbols_tickers.find(v_market_ticker["symbol"].GetString()) != symbols_tickers.end())
							{
								symbols_tickers.at(v_market_ticker["symbol"].GetString()).setData(CMarketTicker(
									v_market_ticker["open"].GetDouble(),
									v_market_ticker["high"].GetDouble(),
									v_market_ticker["low"].GetDouble(),
									v_market_ticker["close"].GetDouble(),
									v_market_ticker["amount"].GetDouble(),
									v_market_ticker["vol"].GetDouble(),
									v_market_ticker["count"].GetInt(),
									v_market_ticker["bid"].GetDouble(),
									v_market_ticker["bidSize"].GetDouble(),
									v_market_ticker["ask"].GetDouble(),
									v_market_ticker["askSize"].GetDouble(),
									v_market_ticker["symbol"].GetString()
								));
								symbols_tickers.at(v_market_ticker["symbol"].GetString()).printData();
							}
						}
					}
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1 * 1000));
	}
	

	CServiceModules::GetInstance()->service_cleanup();
	return 0;
}