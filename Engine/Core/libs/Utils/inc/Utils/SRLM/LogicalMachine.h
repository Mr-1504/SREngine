//
// Created by Monika on 15.08.2023.
//

#ifndef SRENGINE_LOGICALMACHINE_H
#define SRENGINE_LOGICALMACHINE_H

#include <Utils/Common/NonCopyable.h>
#include <Utils/Common/Singleton.h>
#include <Utils/Common/Hashes.h>
#include <Utils/ResourceManager/FileWatcher.h>
#include <flat_hash_map/flat_hash_map.hpp>

namespace SR_SRLM_NS {
    static constexpr uint64_t NODE_BOOL_CONDITION = SR_COMPILE_TIME_CRC32_STR("BOOL_CONDITION");
    static constexpr uint64_t NODE_BOOL_AND = SR_COMPILE_TIME_CRC32_STR("BOOL_AND");
    static constexpr uint64_t NODE_BOOL_OR = SR_COMPILE_TIME_CRC32_STR("BOOL_OR");
    static constexpr uint64_t NODE_BOOL_NOT = SR_COMPILE_TIME_CRC32_STR("BOOL_NOT");

    static constexpr uint64_t NODE_INT_PLUS_INT = SR_COMPILE_TIME_CRC32_STR("INT_PLUS_INT");
    static constexpr uint64_t NODE_INT_MINUS_INT = SR_COMPILE_TIME_CRC32_STR("INT_MINUS_INT");
    static constexpr uint64_t NODE_INT_NEGATIVE = SR_COMPILE_TIME_CRC32_STR("INT_NEGATIVE");
    static constexpr uint64_t NODE_INT_TO_FLOAT = SR_COMPILE_TIME_CRC32_STR("INT_TO_FLOAT");

    static constexpr uint64_t NODE_FLOAT_PLUS_FLOAT = SR_COMPILE_TIME_CRC32_STR("FLOAT_PLUS_FLOAT");
    static constexpr uint64_t NODE_FLOAT_MINUS_FLOAT = SR_COMPILE_TIME_CRC32_STR("FLOAT_MINUS_FLOAT");
    static constexpr uint64_t NODE_FLOAT_NEGATIVE = SR_COMPILE_TIME_CRC32_STR("FLOAT_NEGATIVE");
    static constexpr uint64_t NODE_FLOAT_TO_INT = SR_COMPILE_TIME_CRC32_STR("FLOAT_TO_INT");

    static constexpr uint64_t NODE_LINK_DOT = SR_COMPILE_TIME_CRC32_STR("LINK_DOT");

    static constexpr uint64_t DATA_TYPE_BOOL = SR_COMPILE_TIME_CRC32_STR("BOOL");
    static constexpr uint64_t DATA_TYPE_INT = SR_COMPILE_TIME_CRC32_STR("INT");
    static constexpr uint64_t DATA_TYPE_FLOAT = SR_COMPILE_TIME_CRC32_STR("FLOAT");
    static constexpr uint64_t DATA_TYPE_INT_VECTOR2 = SR_COMPILE_TIME_CRC32_STR("INT_VECTOR2");
    static constexpr uint64_t DATA_TYPE_FLOAT_VECTOR2 = SR_COMPILE_TIME_CRC32_STR("FLOAT_VECTOR2");
    static constexpr uint64_t DATA_TYPE_INT_VECTOR3 = SR_COMPILE_TIME_CRC32_STR("INT_VECTOR3");
    static constexpr uint64_t DATA_TYPE_FLOAT_VECTOR3 = SR_COMPILE_TIME_CRC32_STR("FLOAT_VECTOR3");
    static constexpr uint64_t DATA_TYPE_INT_VECTOR4 = SR_COMPILE_TIME_CRC32_STR("INT_VECTOR4");
    static constexpr uint64_t DATA_TYPE_FLOAT_VECTOR4 = SR_COMPILE_TIME_CRC32_STR("FLOAT_VECTOR4");

    class DataType;
    class DataTypeStruct;

    class DataTypeManager : public SR_UTILS_NS::Singleton<DataTypeManager> {
        friend class SR_UTILS_NS::Singleton<DataTypeManager>;
        using Hash = uint64_t;
    public:
        SR_NODISCARD const std::string& HashToString(Hash hash) const;
        SR_NODISCARD DataType* CreateByName(Hash hashName);

    private:
        void InitSingleton() override;
        void OnSingletonDestroy() override;
        void ReloadSettings();
        void Clear();

    private:
        ska::flat_hash_map<Hash, std::string> m_strings;
        ska::flat_hash_map<Hash, DataTypeStruct*> m_structs;
        SR_UTILS_NS::FileWatcher::Ptr m_watcher;

    };

    class LogicalMachine : public SR_UTILS_NS::NonCopyable {

    };
}

#endif //SRENGINE_LOGICALMACHINE_H