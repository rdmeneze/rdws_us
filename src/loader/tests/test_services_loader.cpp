#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include "../Services/Services.h"

using namespace loader;

class ServicesLoaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir = std::filesystem::temp_directory_path() / "services_test";
        std::filesystem::create_directories(testDir);
    }

    void TearDown() override {
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }

    void createValidServicesJson() {
        validJsonPath = testDir / "valid_services.json";
        std::ofstream file(validJsonPath);
        file << R"([
            {
                "name": "test-service-1",
                "path": "./services/test1",
                "instances": 1
            },
            {
                "name": "test-service-2", 
                "path": "./services/test2",
                "instances": 3
            }
        ])";
        file.close();
    }

    void createInvalidServicesJson() {
        invalidJsonPath = testDir / "invalid_services.json";
        std::ofstream file(invalidJsonPath);
        file << R"([
            {
                "name": "",
                "path": "./services/test1",
                "instances": -1
            }
        ])";
        file.close();
    }

    void createMalformedJson() {
        malformedJsonPath = testDir / "malformed.json";
        std::ofstream file(malformedJsonPath);
        file << "{ invalid json content";
        file.close();
    }

    std::filesystem::path testDir;
    std::filesystem::path validJsonPath;
    std::filesystem::path invalidJsonPath;
    std::filesystem::path malformedJsonPath;
};

TEST_F(ServicesLoaderTest, LoadsValidServicesSuccessfully) {
    createValidServicesJson();

    const Services serviceLoader(validJsonPath);
    
    EXPECT_EQ(serviceLoader.getServiceCount(), 2);
    EXPECT_FALSE(serviceLoader.isEmpty());
    
    const auto& servicesList = serviceLoader.getServices();
    auto it = servicesList.begin();
    
    // Check first service
    EXPECT_EQ(it->getName(), "test-service-1");
    EXPECT_EQ(it->getPath(), "./services/test1");
    EXPECT_EQ(it->getInstances(), 1);
    
    // Check second service
    ++it;
    EXPECT_EQ(it->getName(), "test-service-2");
    EXPECT_EQ(it->getPath(), "./services/test2");
    EXPECT_EQ(it->getInstances(), 3);
}

TEST_F(ServicesLoaderTest, FindsServiceByName) {
    createValidServicesJson();
    
    Services serviceLoader(validJsonPath);
    
    auto* foundService = serviceLoader.findServiceByName("test-service-1");
    ASSERT_NE(foundService, nullptr);
    EXPECT_EQ(foundService->getName(), "test-service-1");
    EXPECT_EQ(foundService->getInstances(), 1);
    
    auto* notFound = serviceLoader.findServiceByName("non-existent");
    EXPECT_EQ(notFound, nullptr);
}

// Test for the bug we just fixed - parsing instances field correctly
TEST_F(ServicesLoaderTest, ParsesInstancesFieldCorrectly) {
    auto testJsonPath = testDir / "instances_test.json";
    std::ofstream file(testJsonPath);
    file << R"([
        {
            "name": "single-instance",
            "path": "./services/single",
            "instances": 1,
            "active": true
        },
        {
            "name": "multi-instance",
            "path": "./services/multi", 
            "instances": 10,
            "active": false
        },
        {
            "name": "zero-instances",
            "path": "./services/zero",
            "instances": 0,
            "active": true
        }
    ])";
    file.close();
    
    Services serviceLoader(testJsonPath);
    
    EXPECT_EQ(serviceLoader.getServiceCount(), 3);
    
    auto* singleService = serviceLoader.findServiceByName("single-instance");
    ASSERT_NE(singleService, nullptr);
    EXPECT_EQ(singleService->getInstances(), 1);
    EXPECT_TRUE(singleService->isActive());
    
    auto* multiService = serviceLoader.findServiceByName("multi-instance");
    ASSERT_NE(multiService, nullptr);
    EXPECT_EQ(multiService->getInstances(), 10);  // This was the bug - always returned 1
    EXPECT_FALSE(multiService->isActive());
    
    auto* zeroService = serviceLoader.findServiceByName("zero-instances");  
    ASSERT_NE(zeroService, nullptr);
    EXPECT_EQ(zeroService->getInstances(), 0);
    EXPECT_TRUE(zeroService->isActive());
}

// Test default values when fields are missing
TEST_F(ServicesLoaderTest, HandlesDefaultValuesCorrectly) {
    auto testJsonPath = testDir / "defaults_test.json";
    std::ofstream file(testJsonPath);
    file << R"([
        {
            "name": "minimal-service",
            "path": "./services/minimal",
            "instances": 1
        },
        {
            "name": "partial-service",
            "path": "./services/partial",
            "instances": 5
        }
    ])";
    file.close();
    
    Services serviceLoader(testJsonPath);
    
    auto* minimalService = serviceLoader.findServiceByName("minimal-service");
    ASSERT_NE(minimalService, nullptr);
    EXPECT_EQ(minimalService->getInstances(), 1);  // Specified value
    EXPECT_FALSE(minimalService->isActive());     // Default value (active field missing)
    
    auto* partialService = serviceLoader.findServiceByName("partial-service");
    ASSERT_NE(partialService, nullptr);
    EXPECT_EQ(partialService->getInstances(), 5);  // Specified value
    EXPECT_FALSE(partialService->isActive());      // Default value (active field missing)
}

// Test edge cases with boolean parsing
TEST_F(ServicesLoaderTest, ParsesBooleanFieldsCorrectly) {
    auto testJsonPath = testDir / "boolean_test.json";
    std::ofstream file(testJsonPath);
    file << R"([
        {
            "name": "active-service",
            "path": "./services/active",
            "instances": 1,
            "active": true
        },
        {
            "name": "inactive-service", 
            "path": "./services/inactive",
            "instances": 2,
            "active": false
        }
    ])";
    file.close();
    
    Services serviceLoader(testJsonPath);
    
    auto* activeService = serviceLoader.findServiceByName("active-service");
    ASSERT_NE(activeService, nullptr);
    EXPECT_TRUE(activeService->isActive());
    
    auto* inactiveService = serviceLoader.findServiceByName("inactive-service");
    ASSERT_NE(inactiveService, nullptr);
    EXPECT_FALSE(inactiveService->isActive());
}

TEST_F(ServicesLoaderTest, ThrowsOnNonExistentFile) {
    std::filesystem::path nonExistentPath = testDir / "does_not_exist.json";
    
    EXPECT_THROW({
        Services serviceLoader(nonExistentPath);
    }, std::runtime_error);
}

TEST_F(ServicesLoaderTest, ThrowsOnMalformedJson) {
    createMalformedJson();
    
    EXPECT_THROW({
        Services serviceLoader(malformedJsonPath);
    }, std::runtime_error);
}

TEST_F(ServicesLoaderTest, ThrowsOnSchemaValidationFailure) {
    createInvalidServicesJson();
    
    EXPECT_THROW({
        Services serviceLoader(invalidJsonPath);
    }, std::runtime_error);
}

TEST_F(ServicesLoaderTest, IteratorWorks) {
    createValidServicesJson();
    
    Services serviceLoader(validJsonPath);
    
    int count = 0;
    for (const auto& svc : serviceLoader) {
        EXPECT_FALSE(svc.getName().empty());
        count++;
    }
    
    EXPECT_EQ(count, 2);
}

TEST_F(ServicesLoaderTest, LoadsEmptyArraySuccessfully) {
    std::filesystem::path emptyJsonPath = testDir / "empty_services.json";
    std::ofstream file(emptyJsonPath);
    file << "[]";  // Empty but valid array
    file.close();
    
    Services serviceLoader(emptyJsonPath);
    
    EXPECT_EQ(serviceLoader.getServiceCount(), 0);
    EXPECT_TRUE(serviceLoader.isEmpty());
}