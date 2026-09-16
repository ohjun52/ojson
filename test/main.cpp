#include <iostream>
#include <json.hpp>

int main()
{       
    ojson::Object A;
    A["nihao"] = {std::string{"Chinese"}};
    A["hello"] = {67.0};
    ojson::Json json{A};

    ojson::Array B{json, {33.2}, {true}};
    ojson::Json json2{B};

    ojson::Array C{json, json2};
    ojson::Json json3{C};
   
    std::cout << json3.get(1).get(0).get("nihao").get<std::string>() << '\n' << json3.get(1).get(1).get<double>() << '\n';
    std::cout << json3.get(1).get(0).get("hello").get<double>() << '\n' << json3.get(1).get(2).get<bool>();
    return 0;
}
