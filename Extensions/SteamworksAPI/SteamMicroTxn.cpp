#include "SteamMicroTxn.h"

inline void SteamMicroTxn::t() {
	{
		std::string body;

		auto c = httplib::SSLClient("https://www.google.com.tw/", 443);

		if (auto res = c.Get("/hi")) {
			if (res->status == 200) {
				std::cout << res->body << std::endl;
			}
		}
		else {
			auto err = res.error();
			body = std::format("HTTP error: {}", httplib::to_string(err));
			OutputDebugStringA(body.c_str());
		}
	}

	//	auto res = cli.Get("/GetUserInfo/v2");
	//	auto a = res->status;
	//	auto b = res->body;

	//	res = cli.Get("/GetUserInfo/v2",
	//	//auto res = cli.Get("/v2/?appid=440&count=3",
	//	  [&] (const char* data, size_t data_length) {
	//				  body.append(data, data_length);
	//				  OutputDebugStringA(body.c_str());

	//				  return true;
	//	  });

	//	a = res->status;
	//	b = res->body;

	//	res = cli.Get(
 // "/GetUserInfo/v2", httplib::Headers(),
 // [&] (const httplib::Response& response) {
	//  return true; // return 'false' if you want to cancel the request.
 // },
 // [&] (const char* data, size_t data_length) {
	//  body.append(data, data_length);
	//  return true; // return 'false' if you want to cancel the request.
 // });

	//	a = res->status;
	//	b = res->body;

	//	return;
}
