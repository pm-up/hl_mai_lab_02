#include "item.h"
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
    void Item::init()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Item` (`id` INT NOT NULL AUTO_INCREMENT,"
                        << "`name` VARCHAR(256) NOT NULL,"
                        << "`category` VARCHAR(256) NOT NULL,"
                        << "`description` VARCHAR(256) NOT NULL,"
                        << "`price` FLOAT NOT NULL,"
                        << "PRIMARY KEY (`id`));",
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

    Poco::JSON::Object::Ptr Item::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("id", _id);
        root->set("name", _name);
        root->set("category", _category);
        root->set("description", _description);
        root->set("price", _price);
        return root;
    }

    Item Item::fromJSON(const std::string &str)
    {
        Item item;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
        item.id() = object->getValue<long>("id");
        item.name() = object->getValue<std::string>("name");
        item.category() = object->getValue<std::string>("category");
        item.description() = object->getValue<std::string>("description");
        item.price() = object->getValue<float>("price");
        return item;
    }

    std::optional<Item> Item::read_by_id(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Item a;
            select << "SELECT id, name, category, description, price FROM Item where id=?",
                into(a._id),
                into(a._name),
                into(a._category),
                into(a._description),
                into(a._price),
                use(id),
                range(0, 1); //  iterate over result set one row at a time

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst()) return a;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl;
        }
        return {};
    }

    std::vector<Item> Item::read_all()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Item> result;
            Item a;
            select << "SELECT id, name, category, description, price FROM Item",
                into(a._id),
                into(a._name),
                into(a._category),
                into(a._description),
                into(a._price),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if (select.execute())
                    result.push_back(a);
            }
            return result;
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

    void Item::save_to_mysql()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);
            insert << "INSERT INTO Item (name,category,description,price) VALUES(?, ?, ?, ?)",
                use(_name),
                use(_category),
                use(_description),
                use(_price);

            insert.execute();
            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(_id),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done())
            {
                select.execute();
            }
            std::cout << "inserted:" << _id << std::endl;
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

    std::vector<Item> Item::search_cart(std::string login)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Item> result;
            Item a;
            select << "SELECT Item.id, name, category, description, price FROM Item INNER JOIN Cart ON Item.id=Cart.id_item WHERE Cart.login=? GROUP BY Cart.id_item",
                into(a._id),
                into(a._name),
                into(a._category),
                into(a._description),
                into(a._price),
                use(login),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if (select.execute())
                    result.push_back(a);
            }

            return result;
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

    long Item::get_id() const
    {
        return _id;
    }
    
    const std::string &Item::get_name() const
    {
        return _name;
    }

    const std::string &Item::get_category() const
    {
        return _category;
    }

    const std::string &Item::get_description() const
    {
        return _description;
    }

    const float &Item::get_price() const
    {
        return _price;
    }

    long &Item::id()
    {
        return _id;
    }

    std::string &Item::name()
    {
        return _name;
    }

    std::string &Item::category()
    {
        return _category;
    }

    std::string &Item::description()
    {
        return _description;
    }

    float &Item::price()
    {
        return _price;
    }

}