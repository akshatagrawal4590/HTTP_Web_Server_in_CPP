# include<iostream>
using namespace std;

// Amit (The Bro Programmer)

class Error
{
    public:
    bool hasError()
    {
        return false;
    }

    string getError()
    {
        return "";
    }
};

class Request
{

};

class Response
{
    public:
    void setContentType(string contentType)
    {
        // Do nothing right now
    }

    Response & operator<<(string content)
    {
        // Do nothing right now
        return *this;
    }
};

class Bro
{
    public:
    void setStaticResourcesFolder(string staticResourcesFolder)
    {
        // Do nothing right now
    }

    void get(string urlPattern, void (*callBack)(Request &, Response &))
    {
        // Do nothing right now
    }

    void listen(int portNo, void (*callBack)(Error &))
    {
        // Do nothing right now
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
        }
        cout<<"Bro HTTP Server is ready to accept requests on Port 6060."<<endl;
    });

    return 0;
}