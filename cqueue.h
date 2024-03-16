#ifndef CQUEUE_H
#define CQUEUE_H

#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <initializer_list>

struct IMemory
{
    virtual void *malloc(size_t size) = 0;
    virtual void free(void *ptr) = 0;
    virtual ~IMemory() = default;
};

template <typename T>
class CQueue
{
    struct node_t
    {
        T data;
        node_t *next{nullptr};
    };

    size_t size{0};
    size_t count{0};
    IMemory &memory;
    node_t *head{nullptr};
    node_t *tail{nullptr};
    static constexpr size_t SIZE_MIN{3};

public:
    CQueue(const CQueue &) = delete;
    CQueue &operator=(const CQueue &) = delete;

    CQueue(IMemory &_memory, size_t _size) : size{_size}, memory{_memory}
    {
        if (size < SIZE_MIN)
        {
            throw std::invalid_argument();
        }

        node_t *node{nullptr};

        for (size_t i = 0; i < size; i++)
        {
            node = static_cast<node_t *>(memory.malloc(sizeof(node_t)));

            if (node == nullptr)
            {
                // Release the memory blocks of the already created nodes
                throw std::bad_alloc();
            }

            (void)new (node) node_t;

            // Link the node
        }

        tail->next = head;
    }

    CQueue(CQueue &&that) noexcept : size{that.size}, count{that.count}, memory{that.memory}, head{that.head}, tail{that.tail}
    {
        that.size = 0;
        that.count = 0;
        that.head = nullptr;
        that.tail = nullptr;
    }

    CQueue &operator=(CQueue &&that) noexcept
    {
        if (this != &that)
        {
            // Release any resource held by this instance
            for (size_t i = 0; i < size; i++)
            {
                tail = head;
                head = head->next;
                tail->~node_t();
                memory.free(tail);
            }

            head = that.head;
            tail = that.tail;
            size = that.size;
            count = that.count;
            memory = that.memory;

            that.size = 0;
            that.count = 0;
            that.head = nullptr;
            that.tail = nullptr;
        }

        return *this;
    }

    void enqueue(const T &item)
    {
        // Store the item in the queue

        // If the queue is full then move head
        // else increment count
    }

    bool dequeue(T &item)
    {
        bool status{false};

        // If the queue is not empty then
        //   set true to status
        //   decrement count
        //   store data in the node pointed by head in item
        //   move head

        return status;
    }

    template <typename U = T, typename = std::enable_if_t<std::is_arithmetic<U>::value>>
    double average(void)
    {
        double value{0.0};

        node_t *cursor{head};

        for (size_t i = 0; i < count; i++)
        {
            value += static_cast<double>(cursor->data);
            cursor = cursor->next;
        }

        return ((count > 0) ? (value / count) : value);
    }

    void resize(size_t _size)
    {
    }

    void clear(void)
    {
        count = 0;
        head = tail->next;
    }

    bool isfull(void) { return (count == size); }

    size_t available(void) { return count; }

    size_t capacity(void) { return size; }

    ~CQueue()
    {
        // Release the memory blocks allocated for the nodes
    }
};

#endif
