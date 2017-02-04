

#include "application.h"
#include "unit-test/unit-test.h"

test(TCP_01_tcp_client_failed_connect_invalid_ip)
{
    TCPClient client;
    client.connect(IPAddress(127,0,0,0), 567);
    client.stop();
}

test(TCP_02_tcp_client_failed_connect_invalid_fqdn)
{
    TCPClient client;
    client.connect("does.not.exist", 567);
    client.stop();
}
