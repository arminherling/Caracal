#pragma once

#include <Caracal/Defines.h>

namespace Caracal
{
    template <typename TValue>
    class ScopedValue final
    {
    public:
        ScopedValue(TValue& slot, TValue newValue) noexcept
            : m_slot{ slot }
            , m_previousValue{ slot }
        {
            m_slot = newValue;
        }

        ~ScopedValue() noexcept
        {
            m_slot = m_previousValue;
        }

        CARACAL_DELETE_COPY_DELETE_MOVE(ScopedValue)

    private:
        TValue& m_slot;
        TValue m_previousValue;
    };
}
