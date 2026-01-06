#parse("SE OSS Header.h")

#[[#pragma]]# once

namespace ${SE_NAMESPACE} {

class ${NAME}
{
public:
    ${NAME}() = default;
    ~${NAME}() = default;
    ${NAME}(const ${NAME}&) = delete;
    ${NAME}(${NAME}&&) = delete;
    ${NAME}& operator=(const ${NAME}&) = delete;
    ${NAME}& operator=(${NAME}&&) = delete;
    
private:

};

} // namespace ${SE_NAMESPACE}
