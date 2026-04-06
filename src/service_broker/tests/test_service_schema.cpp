#include <gtest/gtest.h>
#include <json/json.h>
#include <memory>
#include "validator/schema_validator.h"
#include "schemas/service.h"

using namespace rdws::validation;
using namespace servicebroker::schemas;

class ServiceSchemaTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create validators for both schemas using factory methods
        serviceValidator = std::make_unique<SchemaValidator>(
            SchemaValidator::fromString("service", SERVICE_SCHEMA)
        );
        servicesArrayValidator = std::make_unique<SchemaValidator>(
            SchemaValidator::fromString("services_array", SERVICES_ARRAY_SCHEMA)
        );
    }

    std::unique_ptr<SchemaValidator> serviceValidator;
    std::unique_ptr<SchemaValidator> servicesArrayValidator;
};

TEST_F(ServiceSchemaTest, ValidatesSingleServiceObject) {
    Json::Value validService;
    validService["name"] = "test-service";
    validService["path"] = "./services/test";
    validService["instances"] = 2;

    EXPECT_TRUE(serviceValidator->isValid(validService));
    EXPECT_TRUE(serviceValidator->validate(validService).empty());
}

TEST_F(ServiceSchemaTest, RejectsServiceWithMissingName) {
    Json::Value invalidService;
    invalidService["path"] = "./services/test";
    invalidService["instances"] = 1;

    EXPECT_FALSE(serviceValidator->isValid(invalidService));
    
    auto errors = serviceValidator->validate(invalidService);
    EXPECT_FALSE(errors.empty());
}

TEST_F(ServiceSchemaTest, RejectsServiceWithInvalidInstancesCount) {
    Json::Value invalidService;
    invalidService["name"] = "test-service";
    invalidService["path"] = "./services/test";
    invalidService["instances"] = -1; // Invalid: negative instances

    EXPECT_FALSE(serviceValidator->isValid(invalidService));
    
    auto errors = serviceValidator->validate(invalidService);
    EXPECT_FALSE(errors.empty());
}

TEST_F(ServiceSchemaTest, ValidatesServicesArray) {
    Json::Value servicesArray(Json::arrayValue);
    
    Json::Value service1;
    service1["name"] = "service-test1";
    service1["path"] = "./services/test1";
    service1["instances"] = 1;
    
    Json::Value service2;
    service2["name"] = "service-test2";
    service2["path"] = "./services/test2";
    service2["instances"] = 1;
    
    servicesArray.append(service1);
    servicesArray.append(service2);

    EXPECT_TRUE(servicesArrayValidator->isValid(servicesArray));
    EXPECT_TRUE(servicesArrayValidator->validate(servicesArray).empty());
}

TEST_F(ServiceSchemaTest, ValidatesExistingServicesJsonStructure) {
    // Test with the actual structure from services.json
    std::string servicesJsonContent = R"([
        {
            "name": "service-test1",
            "path": "./services/test1", 
            "instances": 1
        },
        {
            "name": "service-test2",
            "path": "./services/test2",
            "instances": 1
        }
    ])";

    auto errors = servicesArrayValidator->validate(servicesJsonContent);
    EXPECT_TRUE(errors.empty()) << "Existing services.json should be valid";
}