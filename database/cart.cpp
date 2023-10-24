#include "cart.h"
#include "database.h"
#include "../config/config.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{
    void Cart::init()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Cart` (`id_cart` INT NOT NULL AUTO_INCREMENT,"
                        << "`id_item` INT NOT NULL,"
                        << "`login` VARCHAR(256) NOT NULL,"
                        << "PRIMARY KEY (`id_cart`));",
                now;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr Cart::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("id_cart", _id_cart);
        root->set("id_item", _id_item);
        root->set("login", _login);
        return root;
    }

    Cart Cart::fromJSON(const std::string &str)
    {
        Cart cart;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
        cart.id_cart() = object->getValue<long>("id_cart");
        cart.id_item() = object->getValue<long>("id_item");
        cart.login() = object->getValue<std::string>("login");
        return cart;
    }

    void Cart::add_item(Cart cart)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);
            insert << "INSERT INTO Cart (id_item, login) VALUES(?, ?)",
                use(cart._id_item),
                use(cart._login);
            insert.execute();
            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(cart._id_cart),
                range(0, 1); //  iterate over result set one row at a time
            if (!select.done())
            {
                select.execute();
            }
            std::cout << "inserted: id=" << cart._id_cart << std::endl;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    void Cart::remove_item(Cart cart)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement remove(session);
            remove << "DELETE FROM Cart WHERE id_item=? AND login=?",
                use(cart._id_item),
                use(cart._login);
            remove.execute();
            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(cart._id_cart),
                range(0, 1); //  iterate over result set one row at a time
            if (!select.done())
            {
                select.execute();
            }
            std::cout << "deleted: id_item=" << cart._id_item << "; login=" << cart._login << std::endl;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    bool Cart::login_found(std::string login)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            std::string tmp;
            select << "SELECT login FROM User where login=?",
                into(tmp),
                use(login);
            select.execute();
            if (tmp.empty()) return false;
            return true;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl; 
        }
        return false;
    }

    bool Cart::id_found(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            std::string tmp;
            select << "SELECT id FROM Item where id=?",
                into(tmp),
                use(id);
            select.execute();
            if (tmp.empty()) return false;
            return true;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl; 
        }
        return false;
    }

    long Cart::get_id_cart() const
    {
        return _id_cart;
    }

    long Cart::get_id_item() const
    {
        return _id_item;
    }
    
    const std::string &Cart::get_login() const
    {
        return _login;
    }

    long &Cart::id_cart()
    {
        return _id_cart;
    }

    long &Cart::id_item()
    {
        return _id_item;
    }

    std::string &Cart::login()
    {
        return _login;
    }

}