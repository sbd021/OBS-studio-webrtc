// Copyright Dr. Alex. Gouaillard (2015, 2020)

#include "CustomWebrtcImpl.h"
#include "restclient-cpp/connection.h"
#include "restclient-cpp/restclient.h"

#include <iostream>
#include <string>

CustomWebrtcImpl::CustomWebrtcImpl()
{
	RestClient::init();
}

CustomWebrtcImpl::~CustomWebrtcImpl()
{
	// Disconnect just in case
	disconnect(false);
}

bool CustomWebrtcImpl::connect(const std::string &publish_api_url,
			       const std::string & /* room */,
			       const std::string & /*stream_name*/,
			       const std::string &token,
			       WebsocketClient::Listener *listener)
{
	this->serverUrl = sanitizeString(publish_api_url);
	this->token = sanitizeString(token);
	this->listener = listener;

	listener->onLogged(0);

	// OK
	return true;
}

bool CustomWebrtcImpl::open(const std::string &sdp,
			    const std::string &video_codec,
			    const std::string &audio_codec,
			    const std::string &stream_name)
{
	std::cout << "WS-OPEN: stream_name: " << stream_name.c_str() << std::endl;

	RestClient::Connection *conn = new RestClient::Connection("");
	RestClient::HeaderFields headers;
	headers["Authorization"] = "Bearer " + this->token;
	headers["Content-Type"] = "application/sdp";
	headers["Accept"] = "application/sdp";
	conn->SetHeaders(headers);
	conn->SetTimeout(5);
	// enable following of redirects (default is off)
	conn->FollowRedirects(true);
	// and limit the number of redirects (default is -1, unlimited)
	conn->FollowRedirects(true, 3);
	RestClient::Response r = conn->post(this->serverUrl, sdp);
	delete conn;
	RestClient::disable();

	if (r.code < 200 || r.code >= 300) {
		std::cerr << "Error querying publishing websocket url" << std::endl;
		std::cerr << "code: " << r.code << std::endl;
		std::cerr << "body: " << r.body.c_str() << std::endl;
		return false;
	}

	std::string answer =
		r.body + std::string("a=x-google-flag:conference\r\n");

	listener->onOpened(answer);

	// OK
	return true;
}

bool CustomWebrtcImpl::trickle(const std::string & /* mid       */,
			       int /* index     */,
			       const std::string & /* candidate */,
			       bool /* last      */)
{
	return true;
}

bool CustomWebrtcImpl::disconnect(bool /* wait */)
{
	return true;
}

std::string CustomWebrtcImpl::sanitizeString(const std::string &s)
{
	std::string _my_s = s;
	size_t p = _my_s.find_first_not_of(" \n\r\t");
	_my_s.erase(0, p);
	p = _my_s.find_last_not_of(" \n\r\t");
	if (p != std::string::npos)
		_my_s.erase(p + 1);
	return _my_s;
}
