#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "HTTPProxy.h"

namespace i2p
{
namespace proxy
{
	void HTTPProxyConnection::parseHeaders(const std::string& h, std::vector<header>& hm) {
		std::string str (h);
		std::string::size_type idx;
		std::string t;
		int i = 0;
		while( (idx=str.find ("\r\n")) != std::string::npos) {
			t=str.substr (0,idx);
			str.erase (0,idx+2);
			if (t == "")
				break;
			idx=t.find(": ");
			if (idx == std::string::npos)
			{
				std::cout << "Bad header line: " << t << std::endl;
				break;
			}
			LogPrint ("Name: ", t.substr (0,idx), " Value: ", t.substr (idx+2));
			hm[i].name = t.substr (0,idx);
			hm[i].value = t.substr (idx+2);
			i++;
		}
	}

	void HTTPProxyConnection::ExtractRequest(request &m_Request)
	{
		std::string requestString = m_Buffer;
		int idx=requestString.find(" ");
		std::string method = requestString.substr(0,idx);
		requestString = requestString.substr(idx+1);
		idx=requestString.find(" ");
		std::string requestUrl = requestString.substr(0,idx);
		LogPrint("method is: ", method, "\nRequest is: ", requestUrl);
		std::string server="";
		std::string port="80";
		boost::regex rHTTP("http://(.*?)(:(\\d+))?(/.*)");
		boost::smatch m;
		std::string path;
		if(boost::regex_search(requestUrl, m, rHTTP, boost::match_extra)) {
			server=m[1].str();
			if(m[2].str() != "") {
				port=m[3].str();
			}
			path=m[4].str();
		}
		LogPrint("server is: ",server, "\n path is: ",path);
		m_Request.uri = path;
		m_Request.method = method;
		m_Request.host = server;
	}


	void HTTPProxyConnection::RunRequest()
	{
		request m_Request;
		ExtractRequest(m_Request);
		parseHeaders(m_Buffer, m_Request.headers);

		LogPrint("Requesting ", m_Request.host, " with path ", m_Request.uri, " and method ", m_Request.method);
		HandleDestinationRequest(m_Request.host, m_Request.uri);
	}

	//void HTTPConnection::HandleDestinationRequest (const std::string& address, const std::string& uri)
	//{
	//	i2p::data::IdentHash destination;
	//	std::string fullAddress;
	//	if (address.find (".b32.i2p") != std::string::npos)
	//	{
	//		int li = address.find_first_of (".");
	//		std::string newaddress = address.substr (0, li);
	//		if (i2p::data::Base32ToByteStream (newaddress.c_str (), newaddress.length (), (uint8_t *)destination, 32) != 32)
	//		{
	//			LogPrint ("Invalid Base32 address ", newaddress);
	//			return;
	//		}
	//		fullAddress = newaddress + ".b32.i2p";
	//	}
	//	else
	//	{	
	//		auto addr = i2p::data::netdb.FindAddress (address);
	//		if (!addr) 
	//		{
	//			LogPrint ("Unknown address ", address);
	//			SendReply("<html>"+ i2p::proxy::itoopieImage +"<br>Unknown address " + address + "</html>");
	//			return;
	//		}	
	//		destination = *addr;
	//		fullAddress = address;
	//	}
	//		
	//	auto leaseSet = i2p::data::netdb.FindLeaseSet (destination);
	//	if (!leaseSet || !leaseSet->HasNonExpiredLeases ())
	//	{
	//		i2p::data::netdb.Subscribe(destination);
	//		std::this_thread::sleep_for (std::chrono::seconds(10)); // wait for 10 seconds
	//		leaseSet = i2p::data::netdb.FindLeaseSet (destination);
	//		if (!leaseSet || !leaseSet->HasNonExpiredLeases ()) // still no LeaseSet
	//		{
	//			SendReply(leaseSet ? "<html>"+ i2p::proxy::itoopieImage +"<br>Leases expired</html>" : "<html>"+ i2p::proxy::itoopieImage +"LeaseSet not found</html>");
	//			return;
	//		}	
	//	}	
	//	if (!m_Stream)
	//		m_Stream = i2p::stream::CreateStream (*leaseSet);
	//	if (m_Stream)
	//	{
	//		std::string request = "GET " + uri + " HTTP/1.1\n Host:" + fullAddress + "\n";
	//		m_Stream->Send ((uint8_t *)request.c_str (), request.length (), 10);
	//		AsyncStreamReceive ();
	//	}
	//}

}
}

