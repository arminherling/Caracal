#pragma once

#include <Caracal/IR/Instruction.h>
#include <Caracal/IR/IRParameter.h>
#include <Caracal/Semantic/Type.h>

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace Caracal
{
    class ExternFunction
    {
    public:
        ExternFunction() = default;
        ExternFunction(FunctionId id, std::string name, std::optional<std::string> symbolName, const std::vector<IRParameter>& parameters, Type returnType);

        [[nodiscard]] FunctionId id() const noexcept { return m_id; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] const std::optional<std::string>& symbolName() const noexcept { return m_symbolName; }
        [[nodiscard]] const std::vector<IRParameter>& parameters() const noexcept { return m_parameters; }
        [[nodiscard]] Type returnType() const noexcept { return m_returnType; }

        void addParameter(IRParameter parameter);

    private:
        FunctionId m_id{ -1 };
        std::string m_name;
        std::optional<std::string> m_symbolName;
        std::vector<IRParameter> m_parameters;
        Type m_returnType{ Type::Void() };
    };
}
