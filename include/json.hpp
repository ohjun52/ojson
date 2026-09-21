#ifndef OJSON_MAIN_CLASS_HPP
#define OJSON_MAIN_CLASS_HPP

#include <concepts>
#include <cstdint>
#include <functional>
#include <map>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>
#include <string>

namespace ojson
{
    class Json;
    using Null = std::monostate;
    using Boolean = bool;
    using Number = double;
    using String = std::string;
    using Array = std::vector<Json>;
    using Object = std::map<std::string, Json, std::less<>>;
    
    template<typename T, typename... Ts>
    concept OneOf = (std::same_as<T, Ts> || ...);

    template<typename T>
    concept PrimitiveType = (OneOf<std::decay_t<T>, Null, Boolean, Number, String>);

    template<typename T>
    concept StructuredType = OneOf<std::decay_t<T>, Array, Object>;

    template<typename T>
    concept JsonType = (PrimitiveType<T> || StructuredType<T>);
 
    class Json
    {       
        public:
            enum class NodeType : std::uint8_t{kNull, kBoolean, kNumber, kString, kArray, kObject};

            struct NodeProxy;

            Json();

            template<JsonType T>
            Json(T&&);

            NodeProxy get();
            Json& get(const int);
            Json& get(std::string_view);
                   
            NodeType get_current_type() const;

        private:
            using Node = std::variant<Null, Boolean, Number, String, Array, Object>;

            Node node_;
            NodeType current_type_;

            template<PrimitiveType T>
            T& get();

            void ReloadCurrentType();
    };

    struct ojson::Json::NodeProxy
    {
        public:
            NodeProxy(Boolean& boolean) : boolean(&boolean) {}
            NodeProxy(Number& number) : number(&number) {}
            NodeProxy(String& string) : string(&string) {}

            operator bool&()
            {
                if(boolean == nullptr) throw std::runtime_error("getting a wrong type");
                return *boolean;
            }
            operator double&()
            {
                if(number == nullptr) throw std::runtime_error("getting a wrong type");
                return *number;   
            }                   
            operator std::string&()
            {
                if(string == nullptr) throw std::runtime_error("getting a wrong type");
                return *string;
            }
            template<typename T>
            requires std::is_arithmetic_v<T> && (!std::same_as<std::decay_t<T>, bool>) && (!std::same_as<std::decay_t<T>, double>)
            operator T()
            {
                if(number == nullptr) throw std::runtime_error("getting a wrong type");
                return *number;          
            }
            template<typename T>
            requires std::convertible_to<std::string, T> && (!std::same_as<std::decay_t<T>, std::string>)
            operator T()
            {
                if(string == nullptr) throw std::runtime_error("getting a wrong type");
                return *string;            
            }
            
        private:
            Boolean* boolean = nullptr;
            Number* number = nullptr;
            String* string = nullptr;
    };
   
    inline Json::Json() {current_type_ = NodeType::kNull;}
    
    template<JsonType T>
    Json::Json(T&& val)
    {
        node_ = val;
        ReloadCurrentType();
    }

    inline Json::NodeProxy Json::get()
    {
        switch (current_type_)
        {
            case NodeType::kBoolean:
                return NodeProxy(get<Boolean>());
            case NodeType::kNumber:
                return NodeProxy(get<Number>());
            case NodeType::kString:
                return NodeProxy(get<String>());
            default:
                throw std::runtime_error("unknown type occurred");
        }
    }

    template<PrimitiveType T>
    T& Json::get()
    {
       return std::get<std::decay_t<T>>(node_);
    }

    inline Json& Json::get(const int index)
    {
        Array& t = std::get<Array>(node_);
        if(0 <= index && index < t.size())
            return std::get<Array>(node_).at(index);
        throw std::out_of_range("Invalid access to array, index out of range.");
    }

    inline Json& Json::get(std::string_view key)
    {
        Object& t = std::get<Object>(node_);  
        auto it = t.find(key);
        if(it != t.end())
            return it->second;
        throw std::out_of_range("Invalid access to object, key not found.");
    }

    inline void Json::ReloadCurrentType()
    {
        switch (node_.index())
        {
            case 0:
                current_type_ = NodeType::kNull;
                break;
            case 1:
                current_type_ = NodeType::kBoolean;
                break;
            case 2:
                current_type_ = NodeType::kNumber;
                break;
            case 3:
                current_type_ = NodeType::kString;
                break;
            case 4:
                current_type_ = NodeType::kArray;
                break;
            case 5:
                current_type_ = NodeType::kObject;
                break;
            default:
                throw std::runtime_error("Unknown type occurred when reloading type.");
        }
    }
    
    inline Json::NodeType Json::get_current_type() const {return current_type_;}
}
#endif
