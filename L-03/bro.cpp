#include <iostream>
#include <map>
#include <forward_list>
#include <string.h>
#include <windows.h>
#include <unistd.h>

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

class Bro
{
    private:
    string staticResourcesFolder;
    map<string, void (*)(Request &, Response &)> urlMappings;

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
            urlMappings.insert(pair<string, void (*)(Request &, Response &)>(url, callBack));
        }
    }

    void listen(int portNo, void (*callBack)(Error &))
    {
        WSADATA wsaData;
        WORD ver;
        ver = MAKEWORD(1, 1);
        WSAStartup(ver, &wsaData);
        int serverSocketDescriptor;
        char requestBuffer[4096];
        int requestLength;
        int x;  
        serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(serverSocketDescriptor < 0)
        {
            WSACleanup();
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
            WSACleanup();
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
            WSACleanup();
            Error error("Unable to accept client connections");
            callBack(error);
            return;
        }
        Error error("");
        callBack(error);
        struct sockaddr_in clientSocketInformation;
        int len = sizeof(clientSocketInformation);
        int clientSocketDescriptor;
        while(1)
        {
            clientSocketDescriptor = accept(serverSocketDescriptor, (struct sockaddr *)&clientSocketInformation, &len);
            if(clientSocketDescriptor < 0)
            {
                // Not yet decided
            }
            requestLength = recv(clientSocketDescriptor, requestBuffer, sizeof(requestBuffer), 0);
            if(requestLength > 0)
            {
                for(x = 0; x < requestLength; x++)
                {
                    printf("%c", requestBuffer[x]);
                }
                const char* response = 
                "HTTP/1.1 200 OK\r\n"
                "Connection: close\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: 144\r\n\r\n"
                "<html><head><title>Thinking Machines</title></head>"
                "<body><h1>Thinking Machines</h1>"
                "<h3>We teach more than we promise to teach</h3></body></html>";
                send(clientSocketDescriptor, response, strlen(response), 0);
            }
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