#include <iostream>
#include <map>
#include <forward_list>
#include <string.h>
#include <unistd.h>
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef linux
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

using namespace std;

// Amit (The Bro Programmer)

class Validator
{
    private:
    Validator()
    {

    }

    public:
    static bool isValidMIMEType(string &mimeType)
    {
        // Do nothing right now
        return true;
    }

    static bool isValidPath(string &path)
    {
        // Do nothing right now
        return true;
    } 

    static bool isValidURLFormat(string &url)
    {
        // Do nothing right now
        return true;
    }
};

class Error
{
    private:
    string error;

    public:
    Error(string error)
    {
        this -> error = error;
    }
    
    bool hasError()
    {
        return this -> error.length() > 0;
    }

    string getError()
    {
        return this -> error;
    }
};

class Request
{

};

class Response
{
    private:
    string contentType;
    forward_list<string> content;
    forward_list<string>::iterator contentIterator;
    unsigned long contentLength;

    public:
    Response()
    {
        this -> contentLength = 0;
        this -> contentIterator = this -> content.before_begin();
    }

    ~Response()
    {
        // Don nothing right now
    }

    void setContentType(string contentType)
    {
        if(Validator::isValidMIMEType(contentType)) // To check if the given contentType is valid or not
        {
            this -> contentType = contentType;
        }
    }

    Response & operator<<(string content)
    {
        this -> contentLength += content.length();
        this -> contentIterator = this -> content.insert_after(this -> contentIterator, content);
        return *this;
    }
};

enum __request_method__{
    __GET__, __POST__, __PUT__, __DELETE__, __CONNECT__, __TRACE__, __HEAD__, __OPTIONS__ 
};

typedef struct __url__mapping{
    __request_method__ requestMethod;
    void (*mappedFunction) (Request &, Response &);
}URLMapping;

class Bro
{
    private:
    string staticResourcesFolder;
    map<string, URLMapping> urlMappings;

    public:
    Bro()
    {

    }

    ~Bro()
    {

    }

    void setStaticResourcesFolder(string staticResourcesFolder)
    {
        if(Validator::isValidPath(staticResourcesFolder))
        {
            this -> staticResourcesFolder = staticResourcesFolder;
        }
        else
        {
            // Not yet decided
        }
    }

    void get(string url, void (*callBack)(Request &, Response &))
    {
        if(Validator::isValidURLFormat(url))
        {
            urlMappings.insert(pair<string, URLMapping>(url, {__GET__, callBack}));
        }
    }

    void listen(int portNo, void (*callBack)(Error &))
    {
        #ifdef _WIN32
        WSADATA wsaData;
        WORD ver;
        ver = MAKEWORD(1, 1);
        WSAStartup(ver, &wsaData);
        #endif
        int serverSocketDescriptor;
        char requestBuffer[4097];
        int requestLength;
        int x;  
        serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(serverSocketDescriptor < 0)
        {
            #ifdef _WIN32
            WSACleanup();
            #endif
            Error error("Unable to create socket");
            callBack(error);
            return;
        }
        struct sockaddr_in serverSocketInformation;
        serverSocketInformation.sin_family = AF_INET;
        serverSocketInformation.sin_port = htons(portNo);
        serverSocketInformation.sin_addr.s_addr = htonl(INADDR_ANY);
        int successCode = bind(serverSocketDescriptor, (struct sockaddr *)&serverSocketInformation, sizeof(serverSocketInformation));
        if(successCode < 0)
        {
            close(serverSocketDescriptor);
            #ifdef _WIN32
            WSACleanup();
            #endif
            char a[101];
            sprintf(a, "Unable to bind socket to port : %d", portNo);
            Error error(a);
            callBack(error);
            return;
        }
        successCode = ::listen(serverSocketDescriptor, 10);
        if(successCode < 0)
        {
            close(serverSocketDescriptor);
            #ifdef _WIN32
            WSACleanup();
            #endif
            Error error("Unable to accept client connections");
            callBack(error);
            return;
        }
        Error error("");
        callBack(error);
        struct sockaddr_in clientSocketInformation;
        #ifdef _WIN32
        int len = sizeof(clientSocketInformation);
        #endif
        #ifdef linux
        socklen_t len = sizeof(clientSocketInformation);
        #endif
        int clientSocketDescriptor;
        while(1)
        {
            clientSocketDescriptor = accept(serverSocketDescriptor, (struct sockaddr *)&clientSocketInformation, &len);
            if(clientSocketDescriptor < 0)
            {
                // Not yet decided
            }
            forward_list<string> requestBufferDS;
            forward_list<string> :: iterator requestBufferDSIterator;
            requestBufferDSIterator = requestBufferDS.before_begin();
            int requestBufferDSSize = 0;
            int requestDataCount = 0;
            while (1)
            {
                requestLength = recv(clientSocketDescriptor, requestBuffer, sizeof(requestBuffer) - sizeof(char), 0);
                if(requestLength == 0)
                {
                    break;
                }         
                requestBuffer[requestLength] = '\0';
                requestBufferDSIterator = requestBufferDS.insert_after(requestBufferDSIterator, string(requestBuffer));       
                requestBufferDSSize++;
                requestDataCount += requestLength;
            }
            
            if(requestBufferDSSize > 0)
            {
                char *requestData = new char[requestDataCount+1];
                char *p = requestData;
                const char *q;
                requestBufferDSIterator = requestBufferDS.begin();
                while(requestBufferDSIterator != requestBufferDS.end())
                {
                    q = (*requestBufferDSIterator).c_str();
                    while(*q)
                    {
                        *p = *q;
                        p++;
                        q++;
                    }
                    ++requestBufferDSIterator;
                }
                *p = '\0';
                requestBufferDS.clear();
                cout<<"----- Request data begin -----"<<endl;
                cout<<requestData<<endl;
                cout<<"----- Request data end -------"<<endl;
                // the code to parse the request goes here
                delete [] requestData;
            }
            else
            {
                // Something if no data is recieved
            }
            close(clientSocketDescriptor);
        }
    }
};

// Bobby (The user of Bro Server)

int main()
{
    Bro bro;
    bro.setStaticResourcesFolder("whatever");

    bro.get("/", [](Request &request, Response &response) -> void {
        const char *html = R""""(
            <!DOCTYPE HTML>
            <html lang='en'>
            <head>
            <meta charset='utf-8'>
            <title>Whatever</title>
            </head>
            <body>
            <h1>Welcome</h1>
            <h3>Sone Text</h3>
            <a href='getCustomers'>Customers List</a>
            </body>
            </html>
        )"""";
        response.setContentType("text/html"); // setting MIME type
        response<<html;
    });

    bro.get("/getCustomers", [](Request &request, Response &response) -> void {
        const char *html = R""""(
            <!DOCTYPE HTML>
            <html lang='en'>
            <head>
            <meta charset='utf-8'>
            <title>Whatever</title>
            </head>
            <body>
            <h1>List of Customers</h1>
            <ul>
            <li>Ramesh</li>
            <li>Suresh</li>
            <li>Mohan</li>
            </ul>
            <a href='/'>Home</a>
            </body>
        )"""";
        response.setContentType("text/html"); // setting MIME type
        response<<html;
    }); 

    bro.listen(6060, [](Error &error) -> void {
        if(error.hasError())
        {
            cout<<error.getError();
            return;
        }
        cout<<"Bro HTTP Server is ready to accept requests on Port 6060."<<endl;
    });

    return 0;
}