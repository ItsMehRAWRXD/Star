#include "autonomous_unlimited_code_generator.hpp"
#include <sstream>
#include <algorithm>

namespace CodeGeneration {

// ========================= C++ Language Module =========================

class CPlusPlusModule : public LanguageModule {
public:
    Language getLanguage() const override { return Language::CPlusPlus; }
    
    std::string getFileExtension() const override { return ".cpp"; }
    
    std::vector<std::string> getCommonFrameworks() const override {
        return {"STL", "Boost", "Qt", "POCO", "FLTK", "wxWidgets", "SFML", "OpenGL"};
    }
    
    std::string generateBoilerplate(const GenerationContext& context) override {
        std::string code = "#include <iostream>\n";
        code += "#include <string>\n";
        code += "#include <vector>\n";
        code += "#include <memory>\n\n";
        
        if (context.pattern == CodePattern::CLI) {
            code += "#include <map>\n#include <functional>\n\n";
        }
        
        code += "namespace " + context.projectName + " {\n\n";
        code += "class " + context.projectName + " {\n";
        code += "public:\n";
        code += "    " + context.projectName + "();\n";
        code += "    ~" + context.projectName + "();\n\n";
        code += "    void run();\n";
        code += "};\n\n";
        code += "} // namespace " + context.projectName + "\n";
        
        return code;
    }
    
    std::string generateFunction(const std::string& name, const std::vector<std::string>& parameters, 
                                const std::string& returnType, const std::string& body) override {
        std::string func = returnType + " " + name + "(";
        
        for (size_t i = 0; i < parameters.size(); ++i) {
            func += parameters[i];
            if (i < parameters.size() - 1) func += ", ";
        }
        
        func += ") {\n";
        func += "    " + body + "\n";
        func += "}\n";
        
        return func;
    }
    
    std::string generateClass(const std::string& name, const std::vector<std::string>& methods, 
                             const std::vector<std::string>& properties) override {
        std::string cls = "class " + name + " {\n";
        cls += "private:\n";
        
        for (const auto& prop : properties) {
            cls += "    " + prop + ";\n";
        }
        
        cls += "\npublic:\n";
        cls += "    " + name + "();\n";
        cls += "    ~" + name + "();\n\n";
        
        for (const auto& method : methods) {
            cls += "    " + method + ";\n";
        }
        
        cls += "};\n";
        
        return cls;
    }
    
    std::string generateImports(const std::vector<std::string>& imports) override {
        std::string result;
        for (const auto& import : imports) {
            if (import.front() == '<' || import.front() == '"') {
                result += "#include " + import + "\n";
            } else {
                result += "#include <" + import + ">\n";
            }
        }
        return result;
    }
    
    std::string generateDocumentation(const std::string& description) override {
        return "/**\n * " + description + "\n */\n";
    }
    
    std::string generateTests(const std::string& codeToTest) override {
        return R"(#include <gtest/gtest.h>

TEST(GeneratedCodeTest, BasicTest) {
    // Test generated code
    EXPECT_TRUE(true);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
)";
    }
    
    std::vector<std::string> analyzeCode(const std::string& code) override {
        std::vector<std::string> analysis;
        
        if (code.find("using namespace std") != std::string::npos) {
            analysis.push_back("Consider avoiding 'using namespace std' in headers");
        }
        
        if (code.find("malloc") != std::string::npos) {
            analysis.push_back("Consider using new/delete or smart pointers instead of malloc/free");
        }
        
        return analysis;
    }
    
    std::string optimizeCode(const std::string& code) override {
        std::string optimized = code;
        
        // Replace raw pointers with smart pointers where obvious
        size_t pos = 0;
        while ((pos = optimized.find("new ", pos)) != std::string::npos) {
            // Simple heuristic - suggest using smart pointers
            pos += 4;
        }
        
        return optimized;
    }
};

// ========================= Python Language Module =========================

class PythonModule : public LanguageModule {
public:
    Language getLanguage() const override { return Language::Python; }
    
    std::string getFileExtension() const override { return ".py"; }
    
    std::vector<std::string> getCommonFrameworks() const override {
        return {"Flask", "Django", "FastAPI", "NumPy", "Pandas", "TensorFlow", "PyTorch", "Requests"};
    }
    
    std::string generateBoilerplate(const GenerationContext& context) override {
        std::string code = "#!/usr/bin/env python3\n";
        code += "\"\"\"" + context.description + "\"\"\"\n\n";
        
        if (context.pattern == CodePattern::WebApplication || context.pattern == CodePattern::RestAPI) {
            code += "from flask import Flask, request, jsonify\n";
            code += "from flask_cors import CORS\n\n";
        }
        
        code += "import logging\n";
        code += "import sys\n";
        code += "from typing import Optional, List, Dict, Any\n\n";
        
        code += "class " + context.projectName + ":\n";
        code += "    \"\"\"Main application class\"\"\"\n\n";
        code += "    def __init__(self):\n";
        code += "        self.logger = logging.getLogger(__name__)\n\n";
        code += "    def run(self) -> None:\n";
        code += "        \"\"\"Run the application\"\"\"\n";
        code += "        self.logger.info(\"Starting application\")\n\n";
        
        if (context.pattern == CodePattern::CLI) {
            code += "def main() -> int:\n";
            code += "    \"\"\"Main entry point\"\"\"\n";
            code += "    app = " + context.projectName + "()\n";
            code += "    app.run()\n";
            code += "    return 0\n\n";
            code += "if __name__ == '__main__':\n";
            code += "    sys.exit(main())\n";
        }
        
        return code;
    }
    
    std::string generateFunction(const std::string& name, const std::vector<std::string>& parameters, 
                                const std::string& returnType, const std::string& body) override {
        std::string func = "def " + name + "(";
        
        for (size_t i = 0; i < parameters.size(); ++i) {
            func += parameters[i];
            if (i < parameters.size() - 1) func += ", ";
        }
        
        func += ")";
        if (!returnType.empty()) {
            func += " -> " + returnType;
        }
        func += ":\n";
        func += "    \"\"\"TODO: Add function description\"\"\"\n";
        func += "    " + body + "\n";
        
        return func;
    }
    
    std::string generateClass(const std::string& name, const std::vector<std::string>& methods, 
                             const std::vector<std::string>& properties) override {
        std::string cls = "class " + name + ":\n";
        cls += "    \"\"\"TODO: Add class description\"\"\"\n\n";
        cls += "    def __init__(self):\n";
        cls += "        \"\"\"Initialize " + name + "\"\"\"\n";
        
        for (const auto& prop : properties) {
            cls += "        self." + prop + " = None\n";
        }
        
        cls += "\n";
        
        for (const auto& method : methods) {
            cls += "    def " + method + "(self):\n";
            cls += "        \"\"\"TODO: Implement " + method + "\"\"\"\n";
            cls += "        pass\n\n";
        }
        
        return cls;
    }
    
    std::string generateImports(const std::vector<std::string>& imports) override {
        std::string result;
        for (const auto& import : imports) {
            if (import.find(" from ") != std::string::npos) {
                result += import + "\n";
            } else {
                result += "import " + import + "\n";
            }
        }
        return result;
    }
    
    std::string generateDocumentation(const std::string& description) override {
        return "\"\"\"" + description + "\"\"\"\n";
    }
    
    std::string generateTests(const std::string& codeToTest) override {
        return R"(import pytest
import unittest
from unittest.mock import Mock, patch

class TestGeneratedCode(unittest.TestCase):
    """Test cases for generated code"""
    
    def setUp(self):
        """Set up test fixtures"""
        pass
    
    def test_basic_functionality(self):
        """Test basic functionality"""
        self.assertTrue(True)
    
    def tearDown(self):
        """Clean up after tests"""
        pass

if __name__ == '__main__':
    unittest.main()
)";
    }
    
    std::vector<std::string> analyzeCode(const std::string& code) override {
        std::vector<std::string> analysis;
        
        if (code.find("except:") != std::string::npos) {
            analysis.push_back("Use specific exception types instead of bare except");
        }
        
        if (code.find("eval(") != std::string::npos) {
            analysis.push_back("Avoid using eval() for security reasons");
        }
        
        return analysis;
    }
    
    std::string optimizeCode(const std::string& code) override {
        // Simple optimization suggestions
        return code;
    }
};

// ========================= JavaScript Language Module =========================

class JavaScriptModule : public LanguageModule {
public:
    Language getLanguage() const override { return Language::JavaScript; }
    
    std::string getFileExtension() const override { return ".js"; }
    
    std::vector<std::string> getCommonFrameworks() const override {
        return {"Express", "React", "Vue", "Angular", "Node.js", "Lodash", "Axios", "Socket.io"};
    }
    
    std::string generateBoilerplate(const GenerationContext& context) override {
        std::string code;
        
        if (context.pattern == CodePattern::WebApplication || context.pattern == CodePattern::RestAPI) {
            code += "const express = require('express');\n";
            code += "const cors = require('cors');\n";
            code += "const helmet = require('helmet');\n\n";
            code += "const app = express();\n";
            code += "const PORT = process.env.PORT || 3000;\n\n";
            code += "// Middleware\n";
            code += "app.use(helmet());\n";
            code += "app.use(cors());\n";
            code += "app.use(express.json());\n\n";
        }
        
        code += "/**\n * " + context.description + "\n */\n";
        code += "class " + context.projectName + " {\n";
        code += "    constructor() {\n";
        code += "        this.initialized = false;\n";
        code += "    }\n\n";
        code += "    async run() {\n";
        code += "        console.log('Starting application...');\n";
        code += "        this.initialized = true;\n";
        code += "    }\n";
        code += "}\n\n";
        
        if (context.pattern == CodePattern::WebApplication || context.pattern == CodePattern::RestAPI) {
            code += "app.listen(PORT, () => {\n";
            code += "    console.log(`Server running on port ${PORT}`);\n";
            code += "});\n\n";
        }
        
        code += "module.exports = " + context.projectName + ";\n";
        
        return code;
    }
    
    std::string generateFunction(const std::string& name, const std::vector<std::string>& parameters, 
                                const std::string& returnType, const std::string& body) override {
        std::string func = "function " + name + "(";
        
        for (size_t i = 0; i < parameters.size(); ++i) {
            func += parameters[i];
            if (i < parameters.size() - 1) func += ", ";
        }
        
        func += ") {\n";
        func += "    " + body + "\n";
        func += "}\n";
        
        return func;
    }
    
    std::string generateClass(const std::string& name, const std::vector<std::string>& methods, 
                             const std::vector<std::string>& properties) override {
        std::string cls = "class " + name + " {\n";
        cls += "    constructor() {\n";
        
        for (const auto& prop : properties) {
            cls += "        this." + prop + " = null;\n";
        }
        
        cls += "    }\n\n";
        
        for (const auto& method : methods) {
            cls += "    " + method + "() {\n";
            cls += "        // TODO: Implement " + method + "\n";
            cls += "    }\n\n";
        }
        
        cls += "}\n";
        
        return cls;
    }
    
    std::string generateImports(const std::vector<std::string>& imports) override {
        std::string result;
        for (const auto& import : imports) {
            if (import.find("const ") == 0 || import.find("import ") == 0) {
                result += import + "\n";
            } else {
                result += "const " + import + " = require('" + import + "');\n";
            }
        }
        return result;
    }
    
    std::string generateDocumentation(const std::string& description) override {
        return "/**\n * " + description + "\n */\n";
    }
    
    std::string generateTests(const std::string& codeToTest) override {
        return R"(const jest = require('jest');

describe('Generated Code Tests', () => {
    test('should work correctly', () => {
        expect(true).toBe(true);
    });
    
    test('should handle errors gracefully', () => {
        expect(() => {
            // Test error handling
        }).not.toThrow();
    });
});
)";
    }
    
    std::vector<std::string> analyzeCode(const std::string& code) override {
        std::vector<std::string> analysis;
        
        if (code.find("var ") != std::string::npos) {
            analysis.push_back("Consider using 'let' or 'const' instead of 'var'");
        }
        
        if (code.find("==") != std::string::npos && code.find("===") == std::string::npos) {
            analysis.push_back("Use strict equality (===) instead of loose equality (==)");
        }
        
        return analysis;
    }
    
    std::string optimizeCode(const std::string& code) override {
        return code;
    }
};

// ========================= Java Language Module =========================

class JavaModule : public LanguageModule {
public:
    Language getLanguage() const override { return Language::Java; }
    
    std::string getFileExtension() const override { return ".java"; }
    
    std::vector<std::string> getCommonFrameworks() const override {
        return {"Spring", "Spring Boot", "Hibernate", "JUnit", "Maven", "Gradle", "Jackson", "Apache Commons"};
    }
    
    std::string generateBoilerplate(const GenerationContext& context) override {
        std::string code = "package com." + context.projectName.substr(0, context.projectName.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ")) + ";\n\n";
        
        code += "import java.util.*;\n";
        code += "import java.io.*;\n";
        code += "import java.util.logging.Logger;\n\n";
        
        if (context.pattern == CodePattern::WebApplication || context.pattern == CodePattern::RestAPI) {
            code += "import org.springframework.boot.SpringApplication;\n";
            code += "import org.springframework.boot.autoconfigure.SpringBootApplication;\n";
            code += "import org.springframework.web.bind.annotation.*;\n\n";
            code += "@SpringBootApplication\n";
        }
        
        code += "/**\n * " + context.description + "\n */\n";
        code += "public class " + context.projectName + " {\n";
        code += "    private static final Logger logger = Logger.getLogger(" + context.projectName + ".class.getName());\n\n";
        
        code += "    public " + context.projectName + "() {\n";
        code += "        logger.info(\"Initializing " + context.projectName + "\");\n";
        code += "    }\n\n";
        
        code += "    public void run() {\n";
        code += "        logger.info(\"Running application\");\n";
        code += "    }\n\n";
        
        if (context.pattern == CodePattern::WebApplication || context.pattern == CodePattern::RestAPI) {
            code += "    public static void main(String[] args) {\n";
            code += "        SpringApplication.run(" + context.projectName + ".class, args);\n";
            code += "    }\n";
        } else {
            code += "    public static void main(String[] args) {\n";
            code += "        " + context.projectName + " app = new " + context.projectName + "();\n";
            code += "        app.run();\n";
            code += "    }\n";
        }
        
        code += "}\n";
        
        return code;
    }
    
    std::string generateFunction(const std::string& name, const std::vector<std::string>& parameters, 
                                const std::string& returnType, const std::string& body) override {
        std::string func = "public " + returnType + " " + name + "(";
        
        for (size_t i = 0; i < parameters.size(); ++i) {
            func += parameters[i];
            if (i < parameters.size() - 1) func += ", ";
        }
        
        func += ") {\n";
        func += "    " + body + "\n";
        func += "}\n";
        
        return func;
    }
    
    std::string generateClass(const std::string& name, const std::vector<std::string>& methods, 
                             const std::vector<std::string>& properties) override {
        std::string cls = "public class " + name + " {\n";
        
        for (const auto& prop : properties) {
            cls += "    private " + prop + ";\n";
        }
        
        cls += "\n    public " + name + "() {\n";
        cls += "        // Constructor\n";
        cls += "    }\n\n";
        
        for (const auto& method : methods) {
            cls += "    public void " + method + "() {\n";
            cls += "        // TODO: Implement " + method + "\n";
            cls += "    }\n\n";
        }
        
        cls += "}\n";
        
        return cls;
    }
    
    std::string generateImports(const std::vector<std::string>& imports) override {
        std::string result;
        for (const auto& import : imports) {
            result += "import " + import + ";\n";
        }
        return result;
    }
    
    std::string generateDocumentation(const std::string& description) override {
        return "/**\n * " + description + "\n */\n";
    }
    
    std::string generateTests(const std::string& codeToTest) override {
        return R"(import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.AfterEach;
import static org.junit.jupiter.api.Assertions.*;

public class GeneratedCodeTest {
    
    @BeforeEach
    void setUp() {
        // Set up test fixtures
    }
    
    @Test
    void testBasicFunctionality() {
        // Test basic functionality
        assertTrue(true);
    }
    
    @AfterEach
    void tearDown() {
        // Clean up after tests
    }
}
)";
    }
    
    std::vector<std::string> analyzeCode(const std::string& code) override {
        std::vector<std::string> analysis;
        
        if (code.find("System.out.println") != std::string::npos) {
            analysis.push_back("Consider using a proper logging framework instead of System.out.println");
        }
        
        return analysis;
    }
    
    std::string optimizeCode(const std::string& code) override {
        return code;
    }
};

// ========================= Go Language Module =========================

class GoModule : public LanguageModule {
public:
    Language getLanguage() const override { return Language::Go; }
    
    std::string getFileExtension() const override { return ".go"; }
    
    std::vector<std::string> getCommonFrameworks() const override {
        return {"Gin", "Echo", "Fiber", "GORM", "Cobra", "Viper", "Testify", "Logrus"};
    }
    
    std::string generateBoilerplate(const GenerationContext& context) override {
        std::string code = "package main\n\n";
        
        code += "import (\n";
        code += "    \"fmt\"\n";
        code += "    \"log\"\n";
        
        if (context.pattern == CodePattern::WebApplication || context.pattern == CodePattern::RestAPI) {
            code += "    \"net/http\"\n";
            code += "    \"github.com/gin-gonic/gin\"\n";
        }
        
        code += ")\n\n";
        
        code += "// " + context.description + "\n";
        code += "type " + context.projectName + " struct {\n";
        code += "    initialized bool\n";
        code += "}\n\n";
        
        code += "// New creates a new " + context.projectName + " instance\n";
        code += "func New" + context.projectName + "() *" + context.projectName + " {\n";
        code += "    return &" + context.projectName + "{\n";
        code += "        initialized: false,\n";
        code += "    }\n";
        code += "}\n\n";
        
        code += "// Run starts the application\n";
        code += "func (app *" + context.projectName + ") Run() error {\n";
        code += "    log.Println(\"Starting application...\")\n";
        code += "    app.initialized = true\n";
        
        if (context.pattern == CodePattern::WebApplication || context.pattern == CodePattern::RestAPI) {
            code += "\n    r := gin.Default()\n";
            code += "    r.GET(\"/health\", func(c *gin.Context) {\n";
            code += "        c.JSON(http.StatusOK, gin.H{\"status\": \"ok\"})\n";
            code += "    })\n";
            code += "    return r.Run(\":8080\")\n";
        } else {
            code += "    return nil\n";
        }
        
        code += "}\n\n";
        
        code += "func main() {\n";
        code += "    app := New" + context.projectName + "()\n";
        code += "    if err := app.Run(); err != nil {\n";
        code += "        log.Fatal(err)\n";
        code += "    }\n";
        code += "}\n";
        
        return code;
    }
    
    std::string generateFunction(const std::string& name, const std::vector<std::string>& parameters, 
                                const std::string& returnType, const std::string& body) override {
        std::string func = "func " + name + "(";
        
        for (size_t i = 0; i < parameters.size(); ++i) {
            func += parameters[i];
            if (i < parameters.size() - 1) func += ", ";
        }
        
        func += ")";
        if (!returnType.empty()) {
            func += " " + returnType;
        }
        func += " {\n";
        func += "    " + body + "\n";
        func += "}\n";
        
        return func;
    }
    
    std::string generateClass(const std::string& name, const std::vector<std::string>& methods, 
                             const std::vector<std::string>& properties) override {
        std::string cls = "type " + name + " struct {\n";
        
        for (const auto& prop : properties) {
            cls += "    " + prop + "\n";
        }
        
        cls += "}\n\n";
        
        for (const auto& method : methods) {
            cls += "func (s *" + name + ") " + method + "() {\n";
            cls += "    // TODO: Implement " + method + "\n";
            cls += "}\n\n";
        }
        
        return cls;
    }
    
    std::string generateImports(const std::vector<std::string>& imports) override {
        std::string result = "import (\n";
        for (const auto& import : imports) {
            result += "    \"" + import + "\"\n";
        }
        result += ")\n";
        return result;
    }
    
    std::string generateDocumentation(const std::string& description) override {
        return "// " + description + "\n";
    }
    
    std::string generateTests(const std::string& codeToTest) override {
        return R"(package main

import (
    "testing"
)

func TestBasicFunctionality(t *testing.T) {
    // Test basic functionality
    if false {
        t.Error("Test failed")
    }
}

func BenchmarkPerformance(b *testing.B) {
    for i := 0; i < b.N; i++ {
        // Benchmark code
    }
}
)";
    }
    
    std::vector<std::string> analyzeCode(const std::string& code) override {
        std::vector<std::string> analysis;
        
        if (code.find("panic(") != std::string::npos) {
            analysis.push_back("Consider proper error handling instead of panic");
        }
        
        return analysis;
    }
    
    std::string optimizeCode(const std::string& code) override {
        return code;
    }
};

// ========================= Rust Language Module =========================

class RustModule : public LanguageModule {
public:
    Language getLanguage() const override { return Language::Rust; }
    
    std::string getFileExtension() const override { return ".rs"; }
    
    std::vector<std::string> getCommonFrameworks() const override {
        return {"Tokio", "Serde", "Actix", "Rocket", "Diesel", "Clap", "Reqwest", "Anyhow"};
    }
    
    std::string generateBoilerplate(const GenerationContext& context) override {
        std::string code;
        
        if (context.pattern == CodePattern::WebApplication || context.pattern == CodePattern::RestAPI) {
            code += "use actix_web::{web, App, HttpResponse, HttpServer, Result};\n";
            code += "use serde::{Deserialize, Serialize};\n\n";
        }
        
        code += "use std::error::Error;\n";
        code += "use log::info;\n\n";
        
        code += "/// " + context.description + "\n";
        code += "pub struct " + context.projectName + " {\n";
        code += "    initialized: bool,\n";
        code += "}\n\n";
        
        code += "impl " + context.projectName + " {\n";
        code += "    /// Creates a new " + context.projectName + " instance\n";
        code += "    pub fn new() -> Self {\n";
        code += "        Self {\n";
        code += "            initialized: false,\n";
        code += "        }\n";
        code += "    }\n\n";
        
        code += "    /// Runs the application\n";
        code += "    pub async fn run(&mut self) -> Result<(), Box<dyn Error>> {\n";
        code += "        info!(\"Starting application...\");\n";
        code += "        self.initialized = true;\n";
        
        if (context.pattern == CodePattern::WebApplication || context.pattern == CodePattern::RestAPI) {
            code += "\n        HttpServer::new(|| {\n";
            code += "            App::new()\n";
            code += "                .route(\"/health\", web::get().to(health_check))\n";
            code += "        })\n";
            code += "        .bind(\"127.0.0.1:8080\")?\n";
            code += "        .run()\n";
            code += "        .await?;\n";
        }
        
        code += "        Ok(())\n";
        code += "    }\n";
        code += "}\n\n";
        
        if (context.pattern == CodePattern::WebApplication || context.pattern == CodePattern::RestAPI) {
            code += "async fn health_check() -> Result<HttpResponse> {\n";
            code += "    Ok(HttpResponse::Ok().json(serde_json::json!({\n";
            code += "        \"status\": \"ok\"\n";
            code += "    })))\n";
            code += "}\n\n";
        }
        
        code += "#[tokio::main]\n";
        code += "async fn main() -> Result<(), Box<dyn Error>> {\n";
        code += "    env_logger::init();\n";
        code += "    let mut app = " + context.projectName + "::new();\n";
        code += "    app.run().await\n";
        code += "}\n";
        
        return code;
    }
    
    std::string generateFunction(const std::string& name, const std::vector<std::string>& parameters, 
                                const std::string& returnType, const std::string& body) override {
        std::string func = "fn " + name + "(";
        
        for (size_t i = 0; i < parameters.size(); ++i) {
            func += parameters[i];
            if (i < parameters.size() - 1) func += ", ";
        }
        
        func += ")";
        if (!returnType.empty()) {
            func += " -> " + returnType;
        }
        func += " {\n";
        func += "    " + body + "\n";
        func += "}\n";
        
        return func;
    }
    
    std::string generateClass(const std::string& name, const std::vector<std::string>& methods, 
                             const std::vector<std::string>& properties) override {
        std::string cls = "pub struct " + name + " {\n";
        
        for (const auto& prop : properties) {
            cls += "    " + prop + ",\n";
        }
        
        cls += "}\n\n";
        cls += "impl " + name + " {\n";
        
        for (const auto& method : methods) {
            cls += "    pub fn " + method + "(&self) {\n";
            cls += "        // TODO: Implement " + method + "\n";
            cls += "    }\n\n";
        }
        
        cls += "}\n";
        
        return cls;
    }
    
    std::string generateImports(const std::vector<std::string>& imports) override {
        std::string result;
        for (const auto& import : imports) {
            result += "use " + import + ";\n";
        }
        return result;
    }
    
    std::string generateDocumentation(const std::string& description) override {
        return "/// " + description + "\n";
    }
    
    std::string generateTests(const std::string& codeToTest) override {
        return R"(#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_basic_functionality() {
        assert!(true);
    }

    #[tokio::test]
    async fn test_async_functionality() {
        // Test async code
        assert!(true);
    }
}
)";
    }
    
    std::vector<std::string> analyzeCode(const std::string& code) override {
        std::vector<std::string> analysis;
        
        if (code.find("unwrap()") != std::string::npos) {
            analysis.push_back("Consider proper error handling instead of unwrap()");
        }
        
        if (code.find("clone()") != std::string::npos) {
            analysis.push_back("Review clone() usage for potential performance impact");
        }
        
        return analysis;
    }
    
    std::string optimizeCode(const std::string& code) override {
        return code;
    }
};

// ========================= Module Factory =========================

std::unique_ptr<LanguageModule> createLanguageModule(Language lang) {
    switch (lang) {
        case Language::CPlusPlus:
            return std::make_unique<CPlusPlusModule>();
        case Language::Python:
            return std::make_unique<PythonModule>();
        case Language::JavaScript:
            return std::make_unique<JavaScriptModule>();
        case Language::Java:
            return std::make_unique<JavaModule>();
        case Language::Go:
            return std::make_unique<GoModule>();
        case Language::Rust:
            return std::make_unique<RustModule>();
        default:
            return nullptr;
    }
}

} // namespace CodeGeneration