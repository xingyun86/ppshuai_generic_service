#pragma once

#include <rapidjson/rapidjson.h>
#include <rapidjson/reader.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>

#define CHECK_JSON_NUL(JSON, KEY)  (JSON.HasMember(KEY) && JSON[KEY].IsNull())
#define CHECK_JSON_INT(JSON, KEY)  (JSON.HasMember(KEY) && !JSON[KEY].IsNull() && JSON[KEY].IsInt())
#define CHECK_JSON_BOL(JSON, KEY)  (JSON.HasMember(KEY) && !JSON[KEY].IsNull() && JSON[KEY].IsBool())
#define CHECK_JSON_ARY(JSON, KEY)  (JSON.HasMember(KEY) && !JSON[KEY].IsNull() && JSON[KEY].IsArray())
#define CHECK_JSON_FLT(JSON, KEY)  (JSON.HasMember(KEY) && !JSON[KEY].IsNull() && JSON[KEY].IsFloat())
#define CHECK_JSON_I64(JSON, KEY)  (JSON.HasMember(KEY) && !JSON[KEY].IsNull() && JSON[KEY].IsInt64())
#define CHECK_JSON_DBL(JSON, KEY)  (JSON.HasMember(KEY) && !JSON[KEY].IsNull() && JSON[KEY].IsDouble())
#define CHECK_JSON_OBJ(JSON, KEY)  (JSON.HasMember(KEY) && !JSON[KEY].IsNull() && JSON[KEY].IsObject())
#define CHECK_JSON_STR(JSON, KEY)  (JSON.HasMember(KEY) && !JSON[KEY].IsNull() && JSON[KEY].IsString())

__inline static
std::string JSON_VALUE_2_STRING(rapidjson::Value& v)
{
	rapidjson::StringBuffer sb; rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	return std::move(v.Accept(writer) ? std::string(sb.GetString(), sb.GetSize()) : (""));
}
__inline static
rapidjson::Value& STRING_2_JSON_VALUE(rapidjson::Document& d, const std::string& s)
{
	rapidjson::Value& v = d.SetObject();
	d.Parse(s.data(), s.size());
	return v;
}
__inline static
void JSON_STR_NULL_2_EMPTY(std::string& s)
{
	rapidjson::Document d;
	if (!d.Parse(s.data(), s.size()).HasParseError())
	{
		string_replace_all(s, "\"\"", "null");
	}
}
__inline static
void data_to_json(rapidjson::Value& v, const std::string& data, rapidjson::Document::AllocatorType& allocator)
{
	std::string name = string_reader(data, "; name=\"", "\"");
	std::string filename = string_reader(data, "; filename=\"", "\"");
	if (name.length() > 0 && filename.length() == 0)
	{
		v.AddMember(rapidjson::Document().SetString(name.c_str(), name.length(), allocator), "", allocator);
		if (data.find("Content-Type") == std::string::npos)
		{
			std::string text = string_reader(data, "; name=\"" + name + "\"\r\n", "\r\n");
			v[name.c_str()].SetString(text.c_str(), text.length(), allocator);
		}
	}
	else if (name.length() > 0 && filename.length() > 0)
	{
		v.AddMember(rapidjson::Document().SetString(name.c_str(), name.length(), allocator), "", allocator);
		v[name.c_str()].SetString(filename.c_str(), filename.length(), allocator);
	}
	else if (name.length() == 0 && filename.length() > 0)
	{
		v.AddMember(rapidjson::Document().SetString(filename.c_str(), filename.length(), allocator), "", allocator);
		v[filename.c_str()].SetString(name.c_str(), name.length(), allocator);
	}
	else
	{
		//none
	}
}
__inline static
rapidjson::Value& body_to_json(rapidjson::Document& d, const std::string& strData, const std::string& strSplitter, std::string::size_type stPos = 0)
{
	std::string tmp = ("");
	rapidjson::Value& v = d.SetObject();
	std::string::size_type stIdx = 0;
	std::string::size_type stSize = strData.length();
	if (d.Parse(strData.c_str()).HasParseError())
	{
		while ((stPos = strData.find(strSplitter, stIdx)) != std::string::npos)
		{
			tmp = strData.substr(stIdx, stPos - stIdx);
			if (tmp.length() > 0)
			{
				data_to_json(v, tmp, d.GetAllocator());
			}

			stIdx = stPos + strSplitter.length();
		}

		if (stIdx < stSize)
		{
			tmp = strData.substr(stIdx, stSize - stIdx);
			if (tmp.length() > 0)
			{
				data_to_json(v, tmp, d.GetAllocator());
			}
		}
	}
	return v;
}
std::string url_decode(const std::string& encode)
{
	//std::string encode = "%25aa%E6%A3%89%E8%8A%B1CF";
	std::string decode = "";
	std::string::size_type start_pos = 0;
	std::string::size_type final_pos = 0;
	std::string::size_type count_pos = encode.size();
	while (final_pos < count_pos)
	{
		if ((start_pos = encode.find('%', final_pos)) != std::string::npos)
		{
			decode.append(encode.substr(final_pos, start_pos - final_pos));
			final_pos = start_pos;
			if (final_pos + 3 < count_pos)
			{
				char ch1 = encode.at(final_pos + 1);
				char ch2 = encode.at(final_pos + 2);
				if ((
					(ch1 >= '0' && ch1 <= '9') ||
					(ch1 >= 'a' && ch1 <= 'f') ||
					(ch1 >= 'A' && ch1 <= 'F'))
					&&
					(
						(ch2 >= '0' && ch2 <= '9') ||
						(ch2 >= 'a' && ch2 <= 'f') ||
						(ch2 >= 'A' && ch2 <= 'F'))
					)
				{
					decode.append(1, (uint8_t)std::stoi(encode.substr(final_pos + 1, 2), nullptr, 0x10));
				}
				else
				{
					decode.append(encode.substr(final_pos, 3));
				}
				final_pos = final_pos + 3;
				continue;
			}
		}

		decode.append(encode.substr(final_pos));
		break;
	}
	return std::move(decode);
}
