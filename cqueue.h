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
            head = new node(T());
            tail = head;
            for (size_t i = 1; i < memory; ++i)
            {
                tail_->next = new Node(T());
                tail_ = tail_->next;
            }

            tail->next = head;
        }
    }

    void enqueue(const T &item)
    {
        // malloc only allocates memory for a node_t. It does not construct a node_t;
        node_t *node{static_cast<node_t *>(memory.malloc(sizeof(node_t)))};

        if (node != nullptr)
        {
            // Placement new is used to construct an object in an allocated block of memory.
            (void)new (node) node_t{item, nullptr};

            if (head == nullptr)
            {
                head = node;
                tail = head;
            }
            else
            {
                tail->next = node;
                tail = node;
            }

            status = true;
            count++;
        }

        return status;
    }

    bool dequeue(T &item)
    {
        // If the queue is not empty then
        //   set true to status
        //   decrement count
        //   store data in the node pointed by head in item
        //   move head
        bool status{false};
        if (head != nullptr)
        {
            item = head->data;

            node_t *temp{head};
            head = head->next;
            memory.free(temp);

            if (head == nullptr)
            {
                tail = head;
            }

            status = true;
            count--;
        }
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
    void resize(size_t new_size)
    {
        if (new_size < SIZE_MIN)
        {
            throw std::invalid_argument("New size must be greater than 2");
        }
        std::vector<T> new_buffer(new_size);
        size_t len = std::min(count, new_size);
        for (size_t i = 0; i < len; ++i)
        {
            new_buffer[i] = std::move(buffer[(head + i) % memory]);
        }
        buffer = std::move(new_buffer);
        head = 0;
        tail = len % new_size;
        memory = new_size;
    }

    void clear(void)
    {
        count = 0;
        head = tail->next;
    }

    // Read data from the front of the queue
    bool read(T &item) const
    {
        if (isEmpty())
        {
            return false; // Queue is empty, cannot read
        }
        item = data_[head_];
        return true; // Successfully read data
    }

    // Read data from the front of the queue
    bool read(T &item) const
    {
        if (isEmpty())
        {
            return false; // Queue is empty, cannot read
        }
        item = data_[head_];
        return true; // Successfully read data
    }
    // Write data to the queue
    void write(const T &item)
    {
        if (isFull())
        {
            data_[head_] = item;
            head_ = (head_ + 1) % capacity_;
        }
        else
        {
            data_[tail_] = item;
            tail_ = (tail_ + 1) % capacity_;
            size_++;
        }
    }
    size_t size() const
    {
        return count;
    }

    bool isfull(void)
    {
        return (count == size);
    }

    size_t available(void)
    {
        return count;
    }

    size_t capacity(void) { return size; }

    ~CQueue()
    {
        // Release the memory blocks allocated for the nodes

        void releaseMemory()
        {
            Node *current = head_;
            while (current != nullptr)
            {
                Node *next = current->next;
                delete current;
                current = next;
            }
            head_ = nullptr;
            tail_ = nullptr;
            size_ = 0;
        }
    }
};

#endif