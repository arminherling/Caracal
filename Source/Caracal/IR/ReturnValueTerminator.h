#pragma once

#include <Caracal/IR/Terminator.h>
#include <Caracal/IR/ValueRef.h>

namespace Caracal
{
    class ReturnValueTerminator final : public Terminator
    {
    public:
        explicit ReturnValueTerminator(ValueRef value) noexcept;

        [[nodiscard]] ValueRef value() const noexcept { return m_value; }
        void remapValueIds(const ValueIdMap& remap) noexcept override;

    private:
        ValueRef m_value;
    };
}
