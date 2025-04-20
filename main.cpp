#include <iostream>
#include <stack>
#include <queue>
#include <format>
#include <thread>
#include <chrono>
#include <cassert>
#include <condition_variable>


std::queue<int> q;
std::mutex mtx;
std::condition_variable cv;

void push_gift(int i)
{
    {
    std::unique_lock<std::mutex> lk(mtx);
    q.push(i);
    }
    cv.notify_one();
}

void producer()
{
    for(int i=1;i<100;i++)
    {
        push_gift(i);
    }
    push_gift(0);
}

// void producer()
// {
//     mtx.lock();
//     q.push(1);
//     mtx.unlock();
//     cv.notify_one();
//     mtx.lock();
//     q.push(2);
//     mtx.unlock();
//     cv.notify_one(); 
//     mtx.lock();
//     q.push(3);
//     mtx.unlock();
//     cv.notify_one();
//     mtx.lock();
//     q.push(4);
//     mtx.unlock();
//     cv.notify_one();
//     mtx.lock();
//     q.push(5);
//     mtx.unlock();
//     cv.notify_one();
// }

void consumer(int id)
{
    while(true)
    {
        int i;
        {
            std::unique_lock<std::mutex> lk(mtx);
            cv.wait(lk,[]{return !q.empty();});
            i=q.front();
            if (i==0) break;
            q.pop();
        }
    

        std::cout<<std::format("id{} get{}\n",id,i);
    }
     
}

int main()
{
    std::thread producer_thread(producer);
    std::vector<std::thread> consumer_threads;
    for(int i=0;i<32;i++)
    {
        std::thread consumer_thread(consumer,i);
        consumer_threads.push_back(std::move(consumer_thread));
    }
    for (int i=0;i<32;i++)
    {
        consumer_threads[i].join();
    }


    producer_thread.join();

    return 0;
} 
