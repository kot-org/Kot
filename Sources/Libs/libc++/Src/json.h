#pragma once

#include <kot/heap.h>
#include <kot/cstring.h>
#include <kot/utils/map.h>

#include <kot++/printf.h>

namespace std {

    enum JsonTypes : uint8_t {
        JSON_NULL = 0,
        JSON_NUMBER = 1,
        JSON_STRING = 2,
        JSON_BOOLEAN = 3,
        JSON_ARRAY = 4,
        JSON_OBJECT = 5,
    };

    struct JsonLexer {

        uint64_t index;
        char* buffer;

        void skipUseless() {
            while (buffer[index] == ' ' || buffer[index] == '\n' || buffer[index] == '\r') {
                index++;
            }
        }

    };

    enum JsonParsingCode {
        JSON_SUCCESS,
        JSON_FAILED,
    };

    class JsonValue {
    private:
    public:
        JsonValue() {}
        virtual ~JsonValue() {}
        virtual char* serealize() = 0;
        virtual JsonTypes getType() = 0;
        virtual JsonParsingCode getCode() = 0;
    };

    class JsonNumber : public JsonValue {
    private:
        JsonParsingCode code = JSON_SUCCESS;
        int32_t number;
    public:
        JsonNumber() : JsonValue() {}
        JsonNumber(JsonLexer* lexer);
        char* serealize();
        JsonTypes getType();
        JsonParsingCode getCode();
        int32_t get();
        void set(int32_t num);
        ~JsonNumber() {}
    };

    class JsonString : public JsonValue {
    private:
        char* buffer;
        JsonParsingCode code = JSON_SUCCESS;
    public:
        JsonString() : JsonValue() {}
        JsonString(JsonLexer* lexer);
        char* serealize();
        JsonTypes getType();
        JsonParsingCode getCode();
        char* get();
        void set(char* str);
        ~JsonString() {}
    };

    class JsonBoolean : public JsonValue {
    private:
        JsonParsingCode code = JSON_SUCCESS;
        bool val;
    public:
        JsonBoolean() : JsonValue() {}
        JsonBoolean(JsonLexer* lexer);
        char* serealize();
        JsonTypes getType();
        JsonParsingCode getCode();
        bool get();
        void set(bool val);
        ~JsonBoolean() {}
    };

    class JsonObject : public JsonValue {
    private:
        vector_t* obj = map_create();
        JsonParsingCode code = JSON_SUCCESS;
    public:
        JsonObject(JsonLexer* lexer);
        char* serealize();
        JsonTypes getType();
        JsonParsingCode getCode();
        JsonValue* get(char* key);
        void set(char* key, JsonValue* value);
        ~JsonObject() {}
    };

    class JsonNull : public JsonValue {
    private:
        JsonParsingCode code = JSON_SUCCESS;
    public:
        JsonNull(JsonLexer* lexer);
        char* serealize();
        JsonTypes getType();
        JsonParsingCode getCode();
        ~JsonNull() {}
    };

    class JsonArray : public JsonValue {
    private:
        vector_t* arr = vector_create();
        JsonParsingCode code = JSON_SUCCESS;
    public:
        JsonArray(JsonLexer* lexer);
        char* serealize();
        JsonTypes getType();
        JsonParsingCode getCode();
        JsonValue* get(uint64_t index);
        void set(uint64_t index, JsonValue* value);
        void push(JsonValue* value);
        uint64_t length();
        ~JsonArray() {}
    };

    class JsonParser {
    private:
        JsonLexer* lexer = NULL;
        JsonValue* value = NULL;
        JsonParsingCode code = JSON_SUCCESS;
    public:
        JsonParser(JsonLexer* lexer);
        JsonParser(char* source);
        JsonParsingCode getCode();
        JsonValue* getValue();
    };

}
