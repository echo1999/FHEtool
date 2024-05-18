# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.

cmake_minimum_required(VERSION 3.13)

project(SEALExamples VERSION 4.1.1 LANGUAGES CXX)

# If not called from root CMakeLists.txt
if(NOT DEFINED SEAL_BUILD_EXAMPLES)
    set(SEAL_BUILD_EXAMPLES ON)

    # Import Microsoft SEAL
    find_package(SEAL 4.1.1 EXACT REQUIRED)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
endif()

if(SEAL_BUILD_EXAMPLES)
    # 添加可执行文件
    add_executable(sealexamples)

    # # 设置默认值
    # if(NOT DEFINED METHOD)
    #     set(METHOD "default_method")
    # endif()

    # 将参数传递给 example.cpp 文件
    target_compile_definitions(sealexamples
        PRIVATE
            METHOD=${METHOD}
            # DATA_INTEGER=${DATA_INTEGER}
            # DATA_DECIMAL=${DATA_DECIMAL}
            # MUL_INTEGER=${MUL_INTEGER}
            # MUL_DECIMAL=${MUL_DECIMAL}
            # MUL_TIMES=${MUL_TIMES}
    )

    target_sources(sealexamples
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/examples.cpp
            ${CMAKE_CURRENT_LIST_DIR}/myckks.cpp
            ${CMAKE_CURRENT_LIST_DIR}/mybfv.cpp
            ${CMAKE_CURRENT_LIST_DIR}/mybgv.cpp
            ${CMAKE_CURRENT_LIST_DIR}/bgv_mul.cpp
            ${CMAKE_CURRENT_LIST_DIR}/bfv_mul.cpp
            ${CMAKE_CURRENT_LIST_DIR}/ckks_mul.cpp
            ${CMAKE_CURRENT_LIST_DIR}/bgv_mul_v2.cpp
            ${CMAKE_CURRENT_LIST_DIR}/bfv_mul_v2.cpp
            ${CMAKE_CURRENT_LIST_DIR}/ckks_mul_v2.cpp
            ${CMAKE_CURRENT_LIST_DIR}/ckks_business.cpp
            ${CMAKE_CURRENT_LIST_DIR}/bgv_business.cpp
            ${CMAKE_CURRENT_LIST_DIR}/bgv_serve.cpp
            ${CMAKE_CURRENT_LIST_DIR}/bfv_serve.cpp
            ${CMAKE_CURRENT_LIST_DIR}/ckks_serve.cpp

    )

    if(TARGET SEAL::seal)
        target_link_libraries(sealexamples PRIVATE SEAL::seal)
    elseif(TARGET SEAL::seal_shared)
        target_link_libraries(sealexamples PRIVATE SEAL::seal_shared)
    else()
        message(FATAL_ERROR "Cannot find target SEAL::seal or SEAL::seal_shared")
    endif()


endif()
