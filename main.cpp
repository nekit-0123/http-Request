#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sstream>
#include <memory>

// первое приложение с сетями (никогда их не кодил)

int main()
{
    std::cout<< "URL: ";
    std::string address="";
    std::cin>> address;

    if (address.empty())
    {
        std::cout<<"Inccorect URL";
        return -1;
    }

    const char* str = "GET / HTTP/1.1\r\nHost: ";
    const char* endl = "\r\n\r\n\r\n";
    
    std::stringstream conv;
    long lsizeStr = strlen(str) + address.length() + strlen(endl);
    std::unique_ptr<char[]>  message(new char[lsizeStr]);

    conv<<str<<address<<endl;
    conv.read(message.get(),lsizeStr);

    struct sockaddr_in addr;
    struct hostent* raw_host;
    
    raw_host = gethostbyname(address.c_str());
    if (raw_host == NULL)
    {
        std::cout<<"ERROR, no such host";
        return -1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);

    bcopy((char *)raw_host->h_addr,(char *)&addr.sin_addr, raw_host->h_length);

    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cout<<"Connect error";
        return -2;
    }

    if (send(sock, message.get(), lsizeStr, 0) < 0)
    {
        std::cout<<"Send error";
        return -3;
    }

    static char buf[1];
    FILE * file(nullptr);
    file = fopen("text.html", "wb");
    if (file != nullptr)
    {
        while(1)
        {
            if (recv(sock, buf, sizeof(buf), 0) <=0)
                break;

            fwrite(buf, sizeof(buf), sizeof(char), file);
            std::cout<<buf;
        }
 
        fclose(file);
    }

    return 0;
}
