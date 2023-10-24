#include "item_service/http_item_service.h"

int main(int argc, char*argv[]) 
{
    HTTPWebServer app;
    return app.run(argc, argv);
}