#include <gtest/gtest.h>
#include <json/json.h>
#include "schema_validator.h"

using namespace rdws::validation;

class SchemaValidatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Simple schema for testing
        testSchema = R"({
            "type": "object",
            "properties": {
                "name": {"type": "string"},
                "age": {"type": "integer", "minimum": 0}
            },
            "required": ["name"]
        })";
    }

    std::string testSchema;
};

TEST_F(SchemaValidatorTest, ValidatesCorrectJson) {
    auto validator = SchemaValidator::fromString("test", testSchema);
    
    Json::Value validJson;
    validJson["name"] = "John Doe";
    validJson["age"] = 30;

    EXPECT_TRUE(validator.isValid(validJson));
    EXPECT_TRUE(validator.validate(validJson).empty());
}

TEST_F(SchemaValidatorTest, DetectsMissingRequiredField) {
    auto validator = SchemaValidator::fromString("test", testSchema);
    
    Json::Value invalidJson;
    invalidJson["age"] = 25; // missing required "name" field

    EXPECT_FALSE(validator.isValid(invalidJson));
    
    auto errors = validator.validate(invalidJson);
    EXPECT_FALSE(errors.empty());
    EXPECT_EQ(errors.size(), 1);
    // valijson reports missing required fields at root level
    EXPECT_EQ(errors[0].field, "<root>");
    EXPECT_NE(errors[0].message.find("name"), std::string::npos);
}

TEST_F(SchemaValidatorTest, DetectsWrongType) {
    auto validator = SchemaValidator::fromString("test", testSchema);
    
    Json::Value invalidJson;
    invalidJson["name"] = "Jane";
    invalidJson["age"] = "thirty"; // should be integer

    EXPECT_FALSE(validator.isValid(invalidJson));
    
    auto errors = validator.validate(invalidJson);
    EXPECT_FALSE(errors.empty());
}

TEST_F(SchemaValidatorTest, ValidatesJsonString) {
    auto validator = SchemaValidator::fromString("test", testSchema);
    
    std::string validJsonString = R"({"name": "Alice", "age": 25})";
    std::string invalidJsonString = R"({"age": 25})"; // missing name
    
    EXPECT_TRUE(validator.validate(validJsonString).empty());
    EXPECT_FALSE(validator.validate(invalidJsonString).empty());
}