#include "cqueue.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;

class Memory : public IMemory
{
    std::vector<void *> vec;

public:
    MOCK_METHOD(void *, malloc, (size_t size), (override));
    MOCK_METHOD(void, free, (void *ptr), (override));

    void *allocate(size_t size)
    {
        void *ptr{std::malloc(size)};

        if (ptr != nullptr)
        {
            vec.push_back(ptr);
        }

        return ptr;
    }

    void release(void *ptr)
    {
        vec.erase(std::remove(vec.begin(), vec.end(), ptr), vec.end());
        std::free(ptr);
    }

    virtual ~Memory() { EXPECT_EQ(0, vec.size()); }
};

template <typename T>
class QueueFixture : public ::testing::Test
{
    const std::tuple<
        std::vector<int>,
        std::vector<float>,
        std::vector<std::string>>
        allValues{
            {1, 2, 3, 4, 5},
            {1.5f, 2.5f, 3.5f, 4.5f, 5.5f},
            {"aaa", "bbb", "ccc", "ddd", "eee"}};

protected:
    const std::vector<T> values{std::get<std::vector<T>>(allValues)};

    NiceMock<Memory> mock;
    CQueue<T> *cq{nullptr};

    void SetUp(void) override
    {
        ON_CALL(mock, malloc(_))
            .WillByDefault(Invoke(&mock, &Memory::allocate));

        ON_CALL(mock, free(_))
            .WillByDefault(Invoke(&mock, &Memory::release));

        cq = new (std::nothrow) CQueue<T>{mock, values.size()};

        EXPECT_NE(nullptr, cq);
        EXPECT_EQ(values.size(), cq->capacity());

        for (size_t i = 1; i <= values.size(); i++)
        {
            cq->enqueue(values[i - 1]);
            EXPECT_EQ(i, cq->available());
        }

        EXPECT_TRUE(cq->isfull());
    }

    void TearDown(void) override
    {
        delete cq;
    }
};

using TestTypes = ::testing::Types<int, float, std::string>;
TYPED_TEST_SUITE(QueueFixture, TestTypes);

TYPED_TEST(QueueFixture, testConstruction)
{
    EXPECT_NO_THROW(CQueue<TypeParam>(this->mock, 3));
    EXPECT_THROW(CQueue<TypeParam>(this->mock, 2), std::invalid_argument);

    EXPECT_CALL(this->mock, malloc(_))
        .WillOnce(Invoke(&this->mock, &Memory::allocate))
        .WillOnce(Invoke(&this->mock, &Memory::allocate))
        .WillOnce(Return(nullptr));

    EXPECT_THROW(CQueue<TypeParam>(this->mock, this->values.size()), std::bad_alloc);
}

TYPED_TEST(QueueFixture, testAverage)
{
    if constexpr (std::is_arithmetic<TypeParam>::value)
    {
        double avg{0};
        for (size_t i = 0; i < this->values.size(); i++)
        {
            avg += static_cast<double>(this->values[i]);
        }
        avg /= this->values.size();

        EXPECT_FLOAT_EQ(avg, this->cq->average());
    }
}

// Test clear
// Test overwriting
// Test dequeuing
// Test movable
// Test resize