#pragma once

#define OPENSSL_VERSION_1_1

#include "../utils/utils.h"
#include "huobi_signature.h"

class CMarketTicker {
public:
	CMarketTicker() {}
	CMarketTicker(double _open,
		double _high,
		double _low,
		double _close,
		double _amount,
		double _vol,
		int _count,
		double _bid,
		double _bidSize,
		double _ask,
		double _askSize,
		const std::string& _symbol) :open(_open),
		high(_high),
		low(_low),
		close(_close),
		amount(_amount),
		vol(_vol),
		count(_count),
		bid(_bid),
		bidSize(_bidSize),
		ask(_ask),
		askSize(_askSize),
		symbol(_symbol) {}
	virtual ~CMarketTicker() {}
	void setData(const CMarketTicker& _MarketTicker) {
		open = _MarketTicker.open;
		high = _MarketTicker.high;
		low = _MarketTicker.low;
		close = _MarketTicker.close;
		amount = _MarketTicker.amount;
		vol = _MarketTicker.vol;
		count = _MarketTicker.count;
		bid = _MarketTicker.bid;
		bidSize = _MarketTicker.bidSize;
		ask = _MarketTicker.ask;
		askSize = _MarketTicker.askSize;
		symbol = _MarketTicker.symbol;
	}
	void printData() {
		std::cout << "symbol:" << symbol 
			<< " open:" << open
			<< " high:" << high
			<< " low:" << low
			<< " close:" << close
			<< " amount:" << amount
			<< " vol:" << vol
			<< " count:" << count
			<< " bid:" << bid
			<< " bidSize:" << bidSize
			<< " ask:" << ask
			<< " askSize:" << askSize
			<< std::endl;
	}
public:
	double open;
	double high;
	double low;
	double close;
	double amount;
	double vol;
	int count;
	double bid;
	double bidSize;
	double ask;
	double askSize;
	std::string symbol;
};

class CServiceModules {
#define URI_PATH_SPLITTER "/"
#define URI_MARK_SPLITTER "?"
//////////////////////////////////////////
// HTTP METHOD LIST
#define HTTP_METHOD_GET "GET"
#define HTTP_METHOD_PUT "PUT"
#define HTTP_METHOD_POST "POST"
#define HTTP_METHOD_HEAD "HEAD"
#define HTTP_METHOD_TRACE "TRACE"
#define HTTP_METHOD_PATCH "PATCH"
#define HTTP_METHOD_DELETE "DELETE"
#define HTTP_METHOD_OPTIONS "OPTIONS"
//////////////////////////////////////////

//////////////////////////////////////////
// PATH LIST
#define ACCOUNT_ID	"{account-id}"
#define SYMBOLS_INFO "/v1/common/symbols"
#define MARKET_TICKERS "/market/tickers"
#define CURRENCYS_INFO "/v1/common/currencys"
#define TIMESTAMP_DATA "/v1/common/timestamp"
#define ACCOUNTS_INFO "/v1/account/accounts"
#define ACCOUNT_BALANCE "/v1/account/accounts/" ACCOUNT_ID "/balance"
#define ACCOUNT_HISTORY "/v1/account/history"
#define ORDER_HISTORY "/v1/order/orders"
#define OPENORDERS_LIST "/v1/order/openOrders"

#define ORDER_ORDERS_PLACE "/v1/order/orders/place"//交易下单(支持现货\逐仓杠杆\全仓杠杆)

//////////////////////////////////////////

public:
	CServiceModules() :
		schema("https://"),
		host("api.huobi.pro"),
		accesskey("d19df104-176f23c5-1qdmpe4rty-3ef9c"),
		secretkey("212b10f1-f54a4e03-7d0ea328-feb46"),
		//accesskey("xxxxxx-xxxxxx-xxxxxx-xxxxxx"),
		//secretkey("xxxxxx-xxxxxx-xxxxxx-xxxxxx"),
		state_url("https://status.huobigroup.com/api/v2/summary.json") {}
	virtual ~CServiceModules(){}
private:
	std::string schema;
	std::string host;
	std::string accesskey;
	std::string secretkey;
	std::string state_url;
public:
	void service_startup()
	{
		CURL_GLOBAL_INIT(CURL_GLOBAL_DEFAULT);
	}
	void service_cleanup()
	{
		CURL_GLOBAL_EXIT();
	}
	int service_request(
		std::string& resp,
		const std::string& url,
		const std::string& method,
		const std::string& header_slist = "",
		const std::string& post_fields = "",
		const std::string& user_agents = USERAGENT_DEFAULT
	)
	{
		CHttpTask http_task;
		CTaskInfoData tid(
			std::chrono::system_clock::to_time_t(std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now())),
			url, method, curl_slist_append(nullptr, header_slist.data()), post_fields, user_agents);
		http_task.GetTaskMap()->insert(
			std::map<time_t, CTaskInfoData>::value_type
			(
				tid.time,
				tid
			)
		);
		http_task.request(resp);
		JSON_STR_NULL_2_EMPTY(resp);
#ifdef CURL_DEBUG
		std::cout << resp << std::endl;
#endif
		return 0;
	}
	int huobi_service_request(
		std::string& resp,
		const std::string& path,
		const std::string& method,
		const std::string& params = "",
		bool signature = true
	)
	{
		std::string req_url;
		if (signature)
		{
			req_url = schema + host + path + URI_MARK_SPLITTER + Huobi::ApiSignature::buildSignaturePath(host, accesskey, secretkey, path, method, params);
		}
		else
		{
			req_url = path;
		}
		return service_request(resp, req_url, method);
	}
	int huobi_service_request(
		std::string& resp,
		const std::string& path,
		const std::string& method,
		const std::map<std::string, std::string>& params,
		bool signature = true
	)
	{
		std::string req_url;
		if (signature)
		{
			req_url = schema + host + path + URI_MARK_SPLITTER + Huobi::ApiSignature::buildSignaturePath(host, accesskey, secretkey, path, method, params);
		}
		else
		{
			req_url = path;
		}
		return service_request(resp, req_url, method);
	}

public:

	//获取当前系统状态
	int get_system_state(std::string& resp)
	{
		return huobi_service_request(resp, state_url, HTTP_METHOD_GET, "", false);
	}
	//获取交易对信息
	int get_symbols_info(std::string& resp)
	{
		return huobi_service_request(resp, SYMBOLS_INFO, HTTP_METHOD_GET);
	}
	//获取所有交易对的最新Tickers
	int get_market_tickers(std::string& resp)
	{
		return huobi_service_request(resp, MARKET_TICKERS, HTTP_METHOD_GET);
	}
	//获取币种信息
	int get_currencys_info(std::string& resp)
	{
		return huobi_service_request(resp, CURRENCYS_INFO, HTTP_METHOD_GET);
	}
	//获取时间戳信息
	int get_timestamp_info(std::string& resp)
	{
		return huobi_service_request(resp, TIMESTAMP_DATA, HTTP_METHOD_GET);
	}
	//获取账户信息
	int get_accounts_info(std::string & resp)
	{
		return huobi_service_request(resp, ACCOUNTS_INFO, HTTP_METHOD_GET);
	}
	//获取账户余额
	int get_account_balance(std::string & resp, const std::string & account)
	{
		std::string path = ACCOUNT_BALANCE;
		string_replace_all(path, account, ACCOUNT_ID);
		return huobi_service_request(resp, path, HTTP_METHOD_GET);
	}
	//获取账户流水历史
	int get_account_history(std::string& resp, const std::map<std::string, std::string>& params = { {"account-id", ""}, })
	{
		return huobi_service_request(resp, ACCOUNT_HISTORY, HTTP_METHOD_GET, params);
	}
	//获取交易流水历史
	int get_order_history(std::string& resp, const std::map<std::string, std::string>& params = { {"symbol", ""}, {"states", "filled"}, })
	{
		return huobi_service_request(resp, ORDER_HISTORY, HTTP_METHOD_GET, params);
	}
	//获取交易开单未成交信息
	int get_openorders_list(std::string& resp, const std::map<std::string, std::string>& params = { {"account-id", ""}, {"symbol", ""}, })
	{
		return huobi_service_request(resp, OPENORDERS_LIST, HTTP_METHOD_GET, params);
	}
	//////////////////////////////////////////////////
	//	交易下单
	//	type:订单类型，包括:
	//		buy-market, sell-market, 
	//		buy-limit, sell-limit, 
	//		buy-ioc, sell-ioc, 
	//		buy-limit-maker, sell-limit-maker（说明见下文）, 
	//		buy-stop-limit, sell-stop-limit, 
	//		buy-limit-fok, sell-limit-fok, 
	//		buy-stop-limit-fok, sell-stop-limit-fok	
	//
	//buy-limit-maker
	//当“下单价格” >= “市场最低卖出价”，订单提交后，系统将拒绝接受此订单；
	//	当“下单价格”<“市场最低卖出价”，提交成功后，此订单将被系统接受。
	//	sell - limit - maker
	//	当“下单价格” <= “市场最高买入价”，订单提交后，系统将拒绝接受此订单；
	//	当“下单价格”>“市场最高买入价”，提交成功后，此订单将被系统接受。
	//
	//	Response :
	//{
	//	"data": "59378"
	//}
	//响应数据
	//	返回的主数据对象是一个对应下单单号的字符串。
	//	如client order ID（在24小时内）被复用，节点将返回错误消息invalid.client.order.id
	int post_order_orders_place(std::string& resp, const std::map<std::string, std::string>& params = { 
		{"account-id", ""}, //账户 ID
		{"symbol", ""},//交易对
		{"type", ""},//订单类型
		{"amount", ""},//订单交易量（市价买单为订单交易额）
		{"price", ""},//订单价格（对市价单无效）
		{"source", ""},//现货交易填写“spot-api”，逐仓杠杆交易填写“margin-api”，全仓杠杆交易填写“super-margin-api”
		{"client-order-id", ""},//用户自编订单号（最大长度64个字符，须在24小时内保持唯一性）
		})
	{
		return huobi_service_request(resp, ORDER_ORDERS_PLACE, HTTP_METHOD_POST, params);
	}
public:
	SINGLETON_INSTANCE(CServiceModules)
};
