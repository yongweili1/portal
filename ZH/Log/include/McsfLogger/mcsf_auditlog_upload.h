#ifndef MCSF_AUDITLOG_UPLOAD_H
#define MCSF_AUDITLOG_UPLOAD_H

#include <string>
#include "boost/asio.hpp"
#include "boost/asio/error.hpp"
#include "boost/bind.hpp"
#include "mcsf_logviewer_interface_common.h"
#include <iostream>
#include "mcsf_auditlog_archive.h"

#if defined(WIN32)
#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

namespace Mcsf {
	namespace LogViewerInterface {

	enum ProtocolType
	{
		UDP = 0,
		TCP
	};

	class Client{
	public:
		virtual ~Client(){};
		virtual void stop() = 0;
		virtual void write(const std::string& msg) = 0;
		virtual void handle_write(const boost::system::error_code& error, size_t s/*bytes_sent*/) = 0;
		virtual bool is_dead() = 0;
		virtual bool sendComplete() = 0;
		virtual bool isConnected()
		{
			// Declare and initialize variables

			HANDLE hIcmpFile;
			unsigned long ipaddr = INADDR_NONE;
			DWORD dwRetVal = 0;
			char SendData[] = "Data";
			LPVOID ReplyBuffer = NULL;
			DWORD ReplySize = 0;

			ipaddr = inet_addr(m_serverIp.c_str());
			if (ipaddr == INADDR_NONE) {
				std::cout << "Get WRONG IP address: "<< m_serverIp<< std::endl;
				return false;
			}

			hIcmpFile = IcmpCreateFile();
			if (hIcmpFile == INVALID_HANDLE_VALUE) {
				std::cout << "Unable to open handle."<< std::endl;
				std::cout <<"IcmpCreatefile returned error"<<std::cout;
				return false;
			}
			// Allocate space for at a single reply
			ReplySize = sizeof (ICMP_ECHO_REPLY) + sizeof (SendData) + 8;
			ReplyBuffer = (VOID *) malloc(ReplySize);
			if (ReplyBuffer == NULL) {
				std::cout <<"Unable to allocate memory for reply buffer"<< std::endl;
				return false;
			}

			dwRetVal = IcmpSendEcho2(hIcmpFile, NULL, NULL, NULL,
				ipaddr, SendData, sizeof (SendData), NULL,
				ReplyBuffer, ReplySize, 1000);
			if (dwRetVal != 0)
				return true;
			else 
				return false;
		}

	protected:
		std::string m_serverIp;
	};

	using boost::asio::ip::udp;
	class UdpClient:public Client
	{ 
	public: 
		UdpClient( const std::string& host, const std::string& port) 
			:m_socket(m_io_service, udp::endpoint (udp::v4(), 0)), m_isDead(false),m_isComplete(true) { 
				m_serverIp = host;
				udp::resolver resolver(m_io_service); 
				udp::resolver::query query(udp::v4(), host, port); 
				udp::resolver::iterator itr = resolver.resolve(query);

				m_sender_endpoint = *itr; ;
				m_io_service.run();
		}

		virtual ~UdpClient(){ 
			stop();
		}

		void stop()
		{
			m_socket.close();
		}

		void write(const std::string& msg){

			m_isComplete = false;

			if (!msg.empty()){
				m_socket.async_send_to(
					boost::asio::buffer(msg.c_str(), msg.size()),
					m_sender_endpoint,
					boost::bind(&UdpClient::handle_write, this, 
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
			}
		}

		void handle_write(const boost::system::error_code& error, size_t s/*bytes_sent*/)
		{
			s;
			if (error){
				std::cerr<<"Auditlog client error in handle_write:"<<error<<std::endl;
				m_socket.close();
				m_isDead = true;
			}
			m_isComplete = true;
		}

		bool is_dead()
		{
			return m_isDead;
		}

		bool sendComplete()
		{
			return m_isComplete;
		}
		
	private: 
		boost::asio::io_service m_io_service; 
		udp::socket m_socket; 
		udp::endpoint m_sender_endpoint;
		bool m_isDead;
		bool m_isComplete;
	};


	using boost::asio::ip::tcp;
	class TcpClient:public Client
	{
	public:
		TcpClient(const std::string& host, const std::string& port)
			:m_socket(m_io_service), m_isDead(false),m_isComplete(true),m_isConnected(false)
		{
			m_serverIp = host;

			tcp::resolver resolver(m_io_service); 
			tcp::resolver::query query(tcp::v4(), host, port); 
			tcp::resolver::iterator itr = resolver.resolve(query);

			m_sender_endpoint = *itr; 
			m_socket.async_connect(itr->endpoint(),
				boost::bind(&TcpClient::handle_connect,
				this, _1, itr));
			//boost::asio::io_service::work work(m_io_service);
			m_io_service.run();
		}

		virtual ~TcpClient(){ 
			stop();
		}

		void stop()
		{
			m_socket.close();
		}

		void handle_connect(const boost::system::error_code& ec,
			tcp::resolver::iterator endpoint_iter)
		{
			if (!m_socket.is_open())
			{
				std::cout << "Auditlog client connect timed out\n";
			}

			else if (ec)
			{
				std::cout << "Auditlog client connect error: " << ec.message() << "\n";
				m_socket.close();
				m_isDead = true;
			}
			else
				m_isConnected = true;
		}

		void write(const std::string& msg)
		{
			m_isComplete = false;

			boost::asio::async_write(m_socket, boost::asio::buffer(msg.c_str(), msg.size()),
				boost::bind(&TcpClient::handle_write, this, boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}

		void handle_write(const boost::system::error_code& ec, size_t s)
		{
			s;
			if (ec)
			{
				m_isConnected = false;
				std::cout << "Auditlog client write error: " << ec.message() << "\n";
				m_isDead = true;
				stop();
			}
			m_isComplete = true;
		}

		bool is_dead()
		{
			return m_isDead;
		}

		bool sendComplete()
		{
			return m_isComplete;
		}

		virtual bool isConnected()
		{
			return m_isConnected;
		}

	private:
		boost::asio::io_service m_io_service;
		tcp::socket m_socket;
		tcp::endpoint m_sender_endpoint;
		bool m_isDead;
		bool m_isComplete;
		bool m_isConnected;
	};

    /**
     * AuditlogUpload sends LoggingEvents to a auditlog server.
     *
     **/
    class MCSF_LOGVIEWER_INTERFACE AuditlogUpload : public AuditLogArchive
	{
	public:

        /**
         * Instantiate a AuditlogUpload with auditlog server ip and port number
         * @param ServerIpAddr The IP address of server        
         * @param portNumber An alternative port number. Defaults to the 
         * standard syslog port number (514).
         **/         
        AuditlogUpload(ProtocolType connectionType, const std::string& ServerIpAddr, int portNumber = 514);

        virtual ~AuditlogUpload();

        void uploadLogs();

		void start();

		void stop();

		void setAutoDelete(bool autoDelete);

		bool checkConnectionStatus();

		bool getConnectionStatus();
	
	private:
		boost::shared_ptr<Client> m_client;
		bool m_stopUpload;
		bool m_autoDelete;
		ProtocolType m_connectionType;
		std::string m_serverIpAddr;
		int m_portNum;
		int m_retryConnectTimes;
    };
}} //namespace Mcsf::LogViewerInterface

#endif //WIN32
#endif // MCSF_AUDITLOG_UPLOAD_H
