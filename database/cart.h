#ifndef AUTHOR_H
#define AUTHOR_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class Cart{
        private:
            long _id_cart;
            long _id_item;
            std::string _login;

        public:
            static Cart fromJSON(const std::string &str);

            long             get_id_cart() const;
            long             get_id_item() const;
            const std::string &get_login() const;

            long&        id_cart();
            long&        id_item();
            std::string &login();

            static void init();
            static void add_item(Cart cart);
            static void remove_item(Cart cart);
            static bool id_found(long id);
            static bool login_found(std::string login);

            Poco::JSON::Object::Ptr toJSON() const;
    };
}

#endif