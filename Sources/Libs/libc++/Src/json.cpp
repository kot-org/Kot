#include "json.h"
#include "string.h"

namespace std {

    JsonParser::JsonParser(JsonLexer* lex) {

        lexer = lex;

        lexer->skipUseless();

        switch (lexer->buffer[lexer->index]) {
            case '{':
                value = new JsonObject(lexer);
                break;
            case '[':
                value = new JsonArray(lexer);
                break;
            case '"':
                value = new JsonString(lexer);
                break;
            case 'n':
                value = new JsonNull(lexer);
                break;
            case 't':
            case 'f':
                value = new JsonBoolean(lexer);
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                value = new JsonNumber(lexer);
                break;
            default:
                code = JSON_FAILED;
                return;
        }

        code = value->getCode();

    }

    JsonParser::JsonParser(char* str) {

        lexer = (JsonLexer*) malloc(sizeof(JsonLexer));
        lexer->buffer = str;
        lexer->index = 0;

        lexer->skipUseless();

        switch (lexer->buffer[lexer->index]) {
            case '{':
                value = new JsonObject(lexer);
                break;
            case '[':
                value = new JsonArray(lexer);
                break;
            case '"':
                value = new JsonString(lexer);
                break;
            case 'n':
                value = new JsonNull(lexer);
                break;
            case 't':
            case 'f':
                value = new JsonBoolean(lexer);
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                value = new JsonNumber(lexer);
                break;
            default:
                code = JSON_FAILED;
                return;
        }

        code = value->getCode();

    }

    JsonParsingCode JsonParser::getCode() {
        return this->code;
    }

    JsonValue* JsonParser::getValue() {
        return this->value;
    }

    JsonNull::JsonNull(JsonLexer* lexer) : JsonValue() {
        if (lexer->buffer[lexer->index] == 'n') {
            lexer->index++;
            if (lexer->buffer[lexer->index] == 'u') {
                lexer->index++;
                if (lexer->buffer[lexer->index] == 'l') {
                    lexer->index++;
                    if (lexer->buffer[lexer->index] == 'l') {
                        lexer->index++;
                        return;
                    }
                }
            }
        }
        code = JSON_FAILED;
    }

    char* JsonNull::serealize() {
        return "null";
    }

    JsonParsingCode JsonNull::getCode() {
        return this->code;
    }

    JsonTypes JsonNull::getType() {
        return JSON_NULL;
    }

    JsonBoolean::JsonBoolean(JsonLexer* lexer) : JsonValue() {
        if (lexer->buffer[lexer->index] == 't') {
            lexer->index++;
            if (lexer->buffer[lexer->index] == 'r') {
                lexer->index++;
                if (lexer->buffer[lexer->index] == 'u') {
                    lexer->index++;
                    if (lexer->buffer[lexer->index] == 'e') {
                        lexer->index++;
                        this->val = true;
                        return;
                    }
                }
            }
        } else if (lexer->buffer[lexer->index] == 'f') {
            lexer->index++;
            if (lexer->buffer[lexer->index] == 'a') {
                lexer->index++;
                if (lexer->buffer[lexer->index] == 'l') {
                    lexer->index++;
                    if (lexer->buffer[lexer->index] == 's') {
                        lexer->index++;
                        if (lexer->buffer[lexer->index] == 'e') {
                            lexer->index++;
                            this->val = false;
                            return;
                        }
                    }
                }
            }
        }
        code = JSON_FAILED;
    }

    char* JsonBoolean::serealize() {
        if (this->val == true) {
            return "true";
        } else if (this->val == false) {
            return "false";
        }
    }

    JsonParsingCode JsonBoolean::getCode() {
        return this->code;
    }

    JsonTypes JsonBoolean::getType() {
        return JSON_BOOLEAN;
    }

    bool JsonBoolean::get() {
        return this->val;
    }

    void JsonBoolean::set(bool val) {
        this->val = val;
    }

    JsonNumber::JsonNumber(JsonLexer* lexer) : JsonValue() {
        // todo: implement float
        uint16_t start = lexer->index;
        while (lexer->buffer[lexer->index] >= '0' && lexer->buffer[lexer->index] <= '9') {
            if (lexer->buffer[lexer->index] == '\0') {
                code = JSON_FAILED;
                return;
            }
            lexer->index++;
        }
        uint16_t size = lexer->index - start;
        char* temp = (char*) malloc(size + 1);
        memcpy(temp, (uintptr_t)((uint64_t) lexer->buffer + start), size);
        temp[size] = '\0';
        this->number = atoi(temp);
        free(temp);
    }

    char* JsonNumber::serealize() {
        char* buffer = (char*) malloc(asi(this->number, 10));
        return itoa(this->number, buffer, 10);
    }

    JsonParsingCode JsonNumber::getCode() {
        return this->code;
    }

    JsonTypes JsonNumber::getType() {
        return JSON_NUMBER;
    }

    int32_t JsonNumber::get() {
        return this->number;
    }

    void JsonNumber::set(int32_t num) {
        this->number = num;
    }

    JsonString::JsonString(JsonLexer* lexer) : JsonValue() {
        if (lexer->buffer[lexer->index] != '"') {
            code = JSON_FAILED;
            return;
        }
        lexer->index++;
        uint64_t start = lexer->index;
        while (lexer->buffer[lexer->index] != '"') {
            if (lexer->buffer[lexer->index] == '\0') {
                code = JSON_FAILED;
                return;
            }
            lexer->index++;
        }
        uint64_t size = lexer->index - start;
        this->buffer = (char*) malloc(size + 1);
        memcpy(this->buffer, (uintptr_t)((uint64_t) lexer->buffer + start), size);
        this->buffer[size] = '\0';
        lexer->index++;
    }

    char* JsonString::serealize() {
        StringBuilder* builder = new StringBuilder("\"");
        builder->append(buffer);
        builder->append("\"");
        return builder->toString();
    }

    JsonParsingCode JsonString::getCode() {
        return this->code;
    }

    JsonTypes JsonString::getType() {
        return JSON_STRING;
    }

    char* JsonString::get() {
        return this->buffer;
    }

    void JsonString::set(char* str) {
        this->buffer = str;
    }

    JsonObject::JsonObject(JsonLexer* lexer) : JsonValue() {
        if (lexer->buffer[lexer->index] != '{') {
            code = JSON_FAILED;
            return;
        }
        lexer->index++;
        if (lexer->buffer[lexer->index] == '}') {
            lexer->index++;
            return;
        }
        while (true) {
            if (lexer->buffer[lexer->index] == '\0') {
                code = JSON_FAILED;
                return;
            }
            JsonParser* parser1 = new JsonParser(lexer);
            if (parser1->getCode() == JSON_FAILED || parser1->getValue()->getType() != JSON_STRING) {
                code = JSON_FAILED;
                return;
            }
            JsonString* key = (JsonString*) parser1->getValue();
            lexer->skipUseless();
            if (lexer->buffer[lexer->index] != ':') {
                code = JSON_FAILED;
                return;
            }
            lexer->index++;
            JsonParser* parser2 = new JsonParser(lexer);
            if (parser2->getCode() == JSON_FAILED) {
                code = JSON_FAILED;
                return;
            }
            map_set(this->obj, key->get(), parser2->getValue());
            lexer->skipUseless();
            if (lexer->buffer[lexer->index] == '}') {
                lexer->index++;
                break;
            } else if (lexer->buffer[lexer->index] == ',') {
                lexer->index++;
            } else {
                code = JSON_FAILED;
                return;
            }
        }
    }

    char* JsonObject::serealize() {
        StringBuilder* builder = new StringBuilder("{");
        for (uint64_t i = 0; i < obj->length; i++) {
            char* key = (char*) map_key(obj, i);
            builder->append("\"");
            builder->append(key);
            builder->append("\":");
            JsonValue* val = (JsonValue*) map_geti(obj, i);
            char* deserialized = val->serealize();
            builder->append(deserialized);
            if (val->getType() == JSON_STRING || val->getType() == JSON_NUMBER) {
                free(deserialized);
            }
            if (i != obj->length-1) {
                builder->append(",");
            }
        }
        builder->append("}");
        return builder->toString();
    }

    JsonValue* JsonObject::get(char* key) {
        return (JsonValue*) map_get(obj, key);
    }

    void JsonObject::set(char* key, JsonValue* value) {
        map_set(obj, key, value);
    }

    JsonParsingCode JsonObject::getCode() {
        return this->code;
    }

    JsonTypes JsonObject::getType() {
        return JSON_OBJECT;
    }

    JsonArray::JsonArray(JsonLexer* lexer) : JsonValue() {
        if (lexer->buffer[lexer->index] != '[') {
            code = JSON_FAILED;
            return;
        }
        lexer->index++;
        if (lexer->buffer[lexer->index] == ']') {
            lexer->index++;
            return;
        }
        while (true) {
            if (lexer->buffer[lexer->index] == '\0') {
                code = JSON_FAILED;
                return;
            }
            JsonParser* parser = new JsonParser(lexer);
            if (parser->getCode() == JSON_FAILED) {
                code = JSON_FAILED;
                return;
            }
            vector_push(this->arr, parser->getValue());
            lexer->skipUseless();
            if (lexer->buffer[lexer->index] == ']') {
                lexer->index++;
                break;
            } else if (lexer->buffer[lexer->index] == ',') {
                lexer->index++;
            } else {
                code = JSON_FAILED;
                return;
            }
        }
    }

    JsonValue* JsonArray::get(uint64_t index) {
        return (JsonValue*) vector_get(this->arr, index);
    }

    void JsonArray::set(uint64_t index, JsonValue* value) {
        vector_set(this->arr, index, value);
    }

    void JsonArray::push(JsonValue* value) {
        vector_push(this->arr, value);
    }

    uint64_t JsonArray::length() {
        return this->arr->length;
    }

    char* JsonArray::serealize() {
        StringBuilder* builder = new StringBuilder("[");
        for (uint64_t i = 0; i < length(); i++) {
            if (i != 0) {
                builder->append(",");
            }
            JsonValue* val = get(i);
            char* deserialized = val->serealize();
            builder->append(deserialized);
            if (val->getType() == JSON_STRING || val->getType() == JSON_NUMBER) {
                free(deserialized);
            }
        }
        builder->append("]");
        return builder->toString();
    }

    JsonParsingCode JsonArray::getCode() {
        return this->code;
    }

    JsonTypes JsonArray::getType() {
        return JSON_ARRAY;
    }

}