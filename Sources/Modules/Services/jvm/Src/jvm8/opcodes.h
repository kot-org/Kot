#include <kot/utils/vector.h>

typedef struct {
    size_t byteLength;
} JVM8Instruction;

enum JVM8OpCodes {
    iload = 0x15,
};

class JVM8OpCodesTable {
private:
    vector_t* table;
public:
    JVM8OpCodesTable();
    void set(uint64_t opcode, JVM8Instruction* instr);
};