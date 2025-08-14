#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>

#include <mqtt/async_client.h>
#include <mysql_connection.h>
#include <mysql_driver.h>

#include <cppconn/prepared_statement.h>

const std::string SERVER_ADDRESS("tcp://192.168.111.11");
const std::string CLIENT_ID("subscriber");
const std::string TOPIC("all_data");
const int QOS = 1;
const int TIMEOUT = 10000;

class SubscriberCallback : public virtual mqtt::callback
{
public:
    void connection_lost(const std::string& cause) override
    {
        std::cout << "Connection lost: " << cause << std::endl;
    }

    void message_arrived(mqtt::const_message_ptr message) override
    {
        std::cout << "Message arrived: " << message->get_payload_str() << std::endl;

        std::string commaValues = message->get_payload_str();
        std::string separatedValues[3];
        std::stringstream stringStream(commaValues);
        int counter = 0;
        while (stringStream.good()) {
            std::string substr;
            std::getline(stringStream, substr, ',');
            separatedValues[counter] = substr;
            counter++;
        }

        sql::mysql::MySQL_Driver* driver;
        sql::Connection* con;

        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(SERVER_ADDRESS, "root", "password");
        con->setSchema("weatherData");

        sql::Statement* statement;
        statement = con->createStatement();
        std::cout << "0: " << separatedValues[0] << "\n";
        std::cout << "1: " << separatedValues[1] << "\n";
        std::cout << "2: " << separatedValues[2] << "\n";
        std::cout << "3: " << separatedValues[3] << "\n";

        std::string insertData =
            "INSERT INTO data (temperature, humidity, pressure, lux) VALUES (" + separatedValues[0] + ", " + separatedValues[1] + ", " + separatedValues[2] + ", " + separatedValues[3] + ")";

        statement->execute(insertData);

        //delete separatedValues;
        delete statement;
        delete con;
    }
};

int main(int argc, char* argv[])
{
    mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);

    mqtt::connect_options connOpts;
    connOpts.set_keep_alive_interval(20);
    connOpts.set_clean_session(true);

    try
    {
        SubscriberCallback callback;
        client.set_callback(callback);

        mqtt::token_ptr connectionToken = client.connect(connOpts);
        connectionToken->wait();

        mqtt::token_ptr subToken = client.subscribe(TOPIC, QOS);
        subToken->wait();

        while (true)
        {
            // Wait for messages
            std::this_thread::sleep_for(std::chrono::milliseconds(20000));
        }

        mqtt::token_ptr disconnectionToken = client.disconnect();
        disconnectionToken->wait();
    }
    catch (const mqtt::exception& ex)
    {
        std::cerr << "MQTT Exception: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}