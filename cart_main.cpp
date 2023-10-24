#include "cart_service/http_cart_service.h"

int main(int argc, char*argv[]) 
{
    HTTPWebServer app;
    return app.run(argc, argv);
}