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

class StringUtility
{
    private:
    StringUtility(){}

    public:
    static void toLowerCase(char *str)
    {
        if(str == NULL)
        {
            return;
        }
        for(; *str != '\0'; str++)
        {
            if(*str >= 65 && *str <= 90)
            {
                *str += 32;
            }
        }
    }
};

class HTTPErrorStatusUtility
{
    private:
    HTTPErrorStatusUtility(){}

    public:
    static void sendBadRequestError(int clientSocketDescriptor)
    {
        // Will complete later on
    }

    static void sendHttpVersionNotSupportedError(int clientSocketDescriptor, char* httpVersion)
    {
        // Will complete later on
    }

    static void sendNotFoundError(int clientSocketDescriptor, char *requestURL)
    {
        char content[1000];
        char header[200];
        char response[1200];
        sprintf(content, "<!doctype html><html lang='en'><head><meta charset='utf-8'><title>404 Error</title></head><body>Requested Resource [%s] not found</body>", requestURL);
        int contentLength = strlen(content);
        sprintf(header, "HTTP/1.1 404 Not Found\r\nContent-Type:text/html\nContent-Length:%d\nConnection: close\r\n\r\n", contentLength);
        strcpy(response, header);
        strcat(response, content);
        send(clientSocketDescriptor, response, strlen(response), 0);
    }

    static void sendMethodNotAllowedError(int clientSocketDescriptor, char *method, char *requestURL)
    {
        // Will complete later on
    }
};

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
    private:
    char *method;
    char *requestURL;
    char *httpVersion;
    Request(char *method, char *requestURL, char *httpVersion)
    {
        this -> method = method;
        this -> requestURL = requestURL;
        this -> httpVersion = httpVersion;
    }

    friend class Bro;
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

    friend class HTTPResponseUtility;
};

class HTTPResponseUtility
{
    private:
    HTTPResponseUtility(){}

    public:
    static void sendResponse(int clientSocketDescriptor, Response &response)
    {
        char header[200];
        int contentLength = response.contentLength;
        sprintf(header, "HTTP/1.1 200 OK\r\nContent-Type:text/html\nContent-Length:%d\nConnection: close\r\n\r\n", contentLength);
        send(clientSocketDescriptor, header, strlen(header), 0);
        auto contentIterator = response.content.begin();
        while(contentIterator != response.content.end())
        {
            string str = *contentIterator;
            send(clientSocketDescriptor, str.c_str(), str.length(), 0);
            ++contentIterator;
        }
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
            requestLength = recv(clientSocketDescriptor, requestBuffer, sizeof(requestBuffer) - sizeof(char), 0);
            if(requestLength == 0 || requestLength == -1)
            {
                close(clientSocketDescriptor);
                continue;
            }
            int i;
            char *method, *requestURL, *httpVersion;
            requestBuffer[requestLength] = '\0';

            //Code to parse the first line of the http request starts here
            
            //First line should be REQUEST_METHOD SPACE URI SPACE HTTPVersion CRLF
            method = requestBuffer;
            i = 0;
            while(requestBuffer[i] && requestBuffer[i] != ' ')
            {
                i++;
            }
            if(requestBuffer[i] == '\0')
            {
                HTTPErrorStatusUtility :: sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }
            requestBuffer[i] = '\0';
            i++;
            if(requestBuffer[i] == ' ' || requestBuffer[i] == '\0')
            {
                HTTPErrorStatusUtility :: sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }
            StringUtility :: toLowerCase(method);
            if(!(strcmp(method, "get") == 0 || strcmp(method, "post") == 0 || strcmp(method, "put") == 0 || strcmp(method, "connect") == 0 || strcmp(method, "options") == 0 || strcmp(method, "trace") == 0 || strcmp(method, "head") == 0 || strcmp(method, "delete") == 0))
            {
                HTTPErrorStatusUtility :: sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }

            //If the control reaches here that means the request method has been parsed
            
            requestURL = requestBuffer + i;
            while(requestBuffer[i] && requestBuffer[i] != ' ')
            {
                i++;
            }
            if(requestBuffer[i] == '\0')
            {
                HTTPErrorStatusUtility :: sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }
            requestBuffer[i] = '\0';
            i++;
            if(requestBuffer[i] == ' ' || requestBuffer[i] == '\0')
            {
                HTTPErrorStatusUtility :: sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }

            //If the control reaches here that means the requestURL has been parsed

            httpVersion = requestBuffer + i;
            while(requestBuffer[i] && requestBuffer[i] != '\r' && requestBuffer[i] != '\n')
            {
                i++;
            }
            if(requestBuffer[i] == '\0')
            {
                HTTPErrorStatusUtility :: sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }
            if(requestBuffer[i] == '\r' && requestBuffer[i+1] != '\n')
            {
                HTTPErrorStatusUtility :: sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }
            if(requestBuffer[i] == '\r')
            {
                requestBuffer[i] = '\0';
                i = i + 2;
            }
            else
            {
                requestBuffer[i] = '\0';
                i = i + 1;
            }
            StringUtility :: toLowerCase(httpVersion);
            if(strcmp(httpVersion, "http/1.1") != 0)
            {
                HTTPErrorStatusUtility :: sendHttpVersionNotSupportedError(clientSocketDescriptor, httpVersion);
                close(clientSocketDescriptor);
                continue;
            }

            //If the control reaches here that means the first line have been parsed
            
            auto urlMappingIterator = urlMappings.find(requestURL);
            if(urlMappingIterator == urlMappings.end())
            {
                HTTPErrorStatusUtility :: sendNotFoundError(clientSocketDescriptor, requestURL);
                close(clientSocketDescriptor);
                continue;
            }
            URLMapping urlMapping = urlMappingIterator -> second;
            if(urlMapping.requestMethod == __GET__ && strcmp(method, "get") != 0)
            {
                HTTPErrorStatusUtility :: sendMethodNotAllowedError(clientSocketDescriptor, method, httpVersion);
                close(clientSocketDescriptor);
                continue;
            }

            //Code to parse the header and then the payload if exists starts here
            //Code to parse the header and then the payload if exists ends here

            Request request (method, requestURL, httpVersion);
            Response response;
            urlMapping.mappedFunction(request, response);
            HTTPResponseUtility :: sendResponse(clientSocketDescriptor, response);


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
            <h3>Some Text</h3>
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