#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <chrono>

using namespace std;

void obzac()
{
    cout << "\n//============================================================================//\n\n";
};

const std::vector<std::string> THINGS{ "Gold Coin", "Silver Coin", "Watch", "Trash", "Computer", "Videocard" };

int getRandomNum(int min, int max) {
    const static auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::mt19937_64 generator(seed);
    std::uniform_int_distribution<int> dis(min, max);
    return dis(generator);
}

std::mutex m_pcout;
void pcout(const std::string& text) {
    m_pcout.lock();

    std::this_thread::sleep_for(std::chrono::duration<int, std::deci>(getRandomNum(10, 20)));
    std::cout << text << std::endl;
    m_pcout.unlock();
}

std::mutex m_simple;
int FindSimple(int* counter, const int simple_number) {
    pcout("Calculate started");
    std::vector<int> simple_list{ 2 };
    simple_list.reserve(simple_number);
    int num = 1;
    do {
        num += 2;
        bool b_is_simple = true;
       
        for (auto simple : simple_list) {
            if ((num % simple) == 0) {
                b_is_simple = false;
                break;
            }
        }
        if (b_is_simple) {
            m_simple.lock();
            (*counter)++;
            m_simple.unlock();
            simple_list.push_back(num);
        }
    } while (*counter < simple_number);
    pcout(std::to_string(simple_list.at(simple_number - 1)));
    return simple_list.at(simple_number - 1);
}

void PrintProgress(const int* counter, const int simple_number) {
    do {
        m_simple.lock();
        pcout("Progress:" + std::to_string(*counter) + "/" + std::to_string(simple_number));
        m_simple.unlock();
        std::this_thread::sleep_for(std::chrono::duration<int, std::deci>(1));
    } while (*counter < simple_number);
    m_simple.lock();
    pcout("Progress:" + std::to_string(*counter) + "/" + std::to_string(simple_number));
    m_simple.unlock();
}

std::mutex m_thief;
void BringThingToHome(std::vector<std::pair<std::string, int>>* storage) {
    int counter = 0;
    do {
        std::pair new_thing = { THINGS[getRandomNum(0, int(THINGS.size() - 1))], getRandomNum(0, 1000) };
        m_thief.lock();
        storage->push_back(new_thing);
        pcout("Bring thing:" + new_thing.first + " with price:" + std::to_string(new_thing.second));
        m_thief.unlock();
        std::this_thread::sleep_for(std::chrono::duration<int, std::deci>(1));
        counter++;
    } while (counter < 1000);
}

void StealThingFromHome(std::vector<std::pair<std::string, int>>* storage)
{
    std::this_thread::sleep_for(std::chrono::duration<int, std::deci>(150));
    int counter = 0;
    do {
        int highest_price_index = -1;
        m_thief.lock();
        for (int i = 0; i < storage->size(); i++) {
            if (highest_price_index == -1 || storage->at(i).second > storage->at(highest_price_index).second) {
                highest_price_index = i;
            }
        }
        if (highest_price_index >= 0) {
            pcout("Steal thing:" + storage->at(highest_price_index).first + " with price:" + std::to_string(storage->at(highest_price_index).second));
            storage->erase(storage->begin() + highest_price_index);
        }
        m_thief.unlock();
        std::this_thread::sleep_for(std::chrono::duration<int, std::centi>(5));
        counter++;
    } while (counter < 500);
}


void task1()
{
    std::thread thread1{ pcout, "Test string1" };
    std::thread thread2{ pcout, "Test string2" };
    std::thread thread3{ pcout, "Test string3" };
    thread1.join();
    thread2.join();
    thread3.join();
}


void task2()
{
    int progress = 1;
    int result;
    int simple_number;
    std::cout << "Enter number to find simple:";
    std::cin >> simple_number;
    std::thread progress_thread{ PrintProgress, &progress, simple_number };
    progress_thread.detach();
    std::thread simple_thread([&]() { result = FindSimple(&progress, simple_number); });
    simple_thread.join();
}

 
void task3()
{
    std::vector<std::pair<std::string, int>> storage;
    std::thread host{ BringThingToHome, &storage };
    host.detach();
    std::thread thief{ StealThingFromHome, &storage };
    thief.join();
}

int main()
{
    {
        obzac();
        task1();
        obzac();
    }

    {
        obzac();
       task2();
        obzac();
    }

    {
        obzac();
        task3();
        obzac();
    }
    return 0;
}