//
// Created by yangxiaohong on 2021-12-06.
//

#ifndef XHONGWHEELS_BLOCKINGBUFFER_H
#define XHONGWHEELS_BLOCKINGBUFFER_H

#include <atomic>
#include <math.h>
#include <memory>

namespace xhong {

class CircleBlockingBuffer {
  public:
    using ptr = std::shared_ptr<CircleBlockingBuffer>;
    CircleBlockingBuffer(uint32_t blockingBufferSize) : m_blockingBufferSize(blockingBufferSize) {
        m_buffer = static_cast<char*>(malloc(m_blockingBufferSize));
    }

    ~CircleBlockingBuffer() { free(m_buffer); }
    /**
     * 将线性坐标映射成环形缓存中坐标
     * @param pos
     * @return
     */
    uint32_t getPosInCircle(uint32_t pos) const { return pos & (m_blockingBufferSize - 1); }

    /**
     * 获取已经使用的长度
     * @return
     */
    uint32_t getUsedSize() const {
        std::atomic_thread_fence(std::memory_order_acquire);
        uint32_t size = 0;
        if (m_producedPos >= m_consumedPos) {
            size = m_producedPos - m_consumedPos;
        }
        else {
            size = m_blockingBufferSize + m_producedPos - m_consumedPos;
        }
        return size;
    }

    /**
     * 获取未使用的长度
     * @return
     */
    uint32_t getUnusedSize() const { return m_blockingBufferSize - getUsedSize(); }

    /**
     * 重置
     */
    void reset() { m_consumedPos = m_producedPos = 0; }

    /**
     * 使用n个字节长度
     * @param n
     */
    void consume(uint32_t n) { m_consumedPos = getPosInCircle(m_consumedPos + n); }

    /**
     * 消费n字节长度
     * @param to
     * @param n
     * @return
     */
    uint32_t consume(char* toBuf, uint32_t size) {
        // available bytes to consume.
        uint32_t availSize = std::min(getUsedSize(), size);

        // offset of consumePos to buffer end.
        uint32_t off2End = std::min(availSize, m_blockingBufferSize - m_consumedPos);

        if (off2End == availSize) {
            memcpy(toBuf, m_buffer + m_consumedPos, availSize);
        }
        else {
            // first put the data starting from consumePos until the end of buffer.
            memcpy(toBuf, m_buffer + m_consumedPos, off2End);
            // then put the rest at beginning of the buffer.
            memcpy(toBuf + off2End, m_buffer, availSize - off2End);
        }

        m_consumedPos = getPosInCircle(m_consumedPos + availSize);
        std::atomic_thread_fence(std::memory_order_release);

        return availSize;
    }

    void produce(const char* fromBuf, uint32_t size) {
        while (getUnusedSize() < size) {
            /* blocking */;
        }

        // offset of producePos to buffer end.
        uint32_t off2End = std::min(size, m_blockingBufferSize - m_producedPos);
        if (off2End == size) {
            memcpy(m_buffer + m_producedPos, fromBuf, size);
        }
        else {
            // first put the data starting from producePos until the end of buffer.
            memcpy(m_buffer + m_producedPos, fromBuf, off2End);

            // then put the rest at beginning of the buffer.
            memcpy(m_buffer, fromBuf + off2End, size - off2End);
        }

        m_producedPos = getPosInCircle(m_producedPos + size);
        std::atomic_thread_fence(std::memory_order_release);
    }

  private:
    uint32_t m_blockingBufferSize{2 * 1024 * 1024};
    uint32_t m_producedPos{0};
    uint32_t m_consumedPos{0};
    char*    m_buffer{nullptr};
};
}  // namespace xhong

#endif  // XHONGWHEELS_BLOCKINGBUFFER_H
