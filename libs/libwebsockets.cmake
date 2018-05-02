## Libwebsockets Start
set(PARENT_PROJECT_NAME "ipbc-miner")


if(WIN32)
  message("Using Win static libwebsockets")

  include_directories("${CMAKE_SOURCE_DIR}/libs/libwebsockets_static/include")
  target_link_libraries(${PARENT_PROJECT_NAME} "${CMAKE_SOURCE_DIR}/libs/libwebsockets_static/win64/websockets.lib")
  set(Win_DLLS ${Win_DLLS} "${CMAKE_SOURCE_DIR}/libs/libwebsockets_static/win64/websockets.dll")

elseif(UNIX)
  message("Using Linux static libwebsockets")

  include_directories("${CMAKE_SOURCE_DIR}/libs/libwebsockets_static/linux64/include")
  ##target_link_libraries(${PARENT_PROJECT_NAME} debug "${CMAKE_SOURCE_DIR}/libs/libwebsockets_static/linux64/libwebsockets-debug.a" z)
  target_link_libraries(${PARENT_PROJECT_NAME} "${CMAKE_SOURCE_DIR}/libs/libwebsockets_static/linux64/libwebsockets-release.a" z)
  
  #target_link_libraries(lib_ipbc ${CMAKE_SOURCE_DIR}/third_party/openssl/libssl.a ${CMAKE_SOURCE_DIR}/third_party/openssl/libcrypto.a)
  #target_link_libraries(lib_ipbc ssl crypto crypt dl z)

else()
  message("Using Default libwebsockets")

  #include_directories("${CMAKE_SOURCE_DIR}/third_party/libwebsockets_static/include")
  #target_link_libraries(lib_ipbc debug "${CMAKE_SOURCE_DIR}/third_party/libwebsockets_static/linux64/libwebsockets-debug.a")
  #target_link_libraries(lib_ipbc optimized "${CMAKE_SOURCE_DIR}/third_party/libwebsockets_static/linux64/libwebsockets-release.a")
  #find_package(libwebsockets REQUIRED)

  #--- set(LWS_WITHOUT_TESTAPPS on)
  #--- set(LWS_WITHOUT_TEST_SERVER on)
  #--- set(LWS_WITHOUT_TEST_SERVER_EXTPOLL on)
  #--- set(LWS_WITHOUT_TEST_PING on)
  #--- set(LWS_WITHOUT_TEST_ECHO on)
  #--- set(LWS_WITHOUT_TEST_CLIENT on)
  #--- set(LWS_WITHOUT_TEST_FRAGGLE on)
  #--- set(LWS_IPV6 on)
  #--- set(LWS_WITHOUT_CLIENT on)
  #--- set(LWS_WITH_SHARED off)


  #--- set(LWS_OPENSSL_ROOT_DIR ${CMAKE_SOURCE_DIR}/third_party/openssl/)
  #--- set(LWS_OPENSSL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/third_party/openssl/include/)
  #--- set(OPENSSL_ROOT_DIR ${CMAKE_SOURCE_DIR}/third_party/openssl/)
  #--- set(OPENSSL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third_party/openssl/include/)


  #set(LWS_OPENSSL_ROOT_DIR ${CMAKE_SOURCE_DIR}/third_party/openssl_static/)
  #set(LWS_OPENSSL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/third_party/openssl_static/include/)
  #set(OPENSSL_ROOT_DIR ${CMAKE_SOURCE_DIR}/third_party/openssl_static/)
  #set(OPENSSL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third_party/openssl_static/include/)
  

  #set(OPENSSL_LIBRARIES ${CMAKE_SOURCE_DIR}/third_party/openssl/libssl.so ${CMAKE_SOURCE_DIR}/third_party/openssl/libcrypto.so) #// Get it from libwebrtc
  #set(LWS_OPENSSL_LIBRARIES ${CMAKE_SOURCE_DIR}/third_party/openssl/libssl.so ${CMAKE_SOURCE_DIR}/third_party/openssl/libcrypto.so) #// Get it from libwebrtc

  #set(OPENSSL_LIBRARIES ${CMAKE_SOURCE_DIR}/third_party/openssl_static/linux/libssl.a ${CMAKE_SOURCE_DIR}/third_party/openssl_static/linux/libcrypto.a) #// Get it from libwebrtc
  #set(LWS_OPENSSL_LIBRARIES ${CMAKE_SOURCE_DIR}/third_party/openssl_static/linux/libssl.a ${CMAKE_SOURCE_DIR}/third_party/openssl_static/linux/libcrypto.a) #// Get it from libwebrtc

  #set(OPENSSL_LIBRARIES ${CMAKE_SOURCE_DIR}/third_party/openssl_static/linux/libssl.so ${CMAKE_SOURCE_DIR}/third_party/openssl_static/linux/libcrypto.so) #// Get it from libwebrtc
  #set(LWS_OPENSSL_LIBRARIES ${CMAKE_SOURCE_DIR}/third_party/openssl_static/linux/libssl.so ${CMAKE_SOURCE_DIR}/third_party/openssl_static/linux/libcrypto.so) #// Get it from libwebrtc

  #--- include_directories(${CMAKE_SOURCE_DIR}/third_party/openssl/include/)
  #--- add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/libwebsockets/)
  #--- include_directories(${LIBWEBSOCKETS_INCLUDE_DIRS})
  #--- message("LWS Libraries Static: "  ${LIBWEBSOCKETS_LIBRARIES_STATIC})
  #--- target_link_libraries(lib_ipbc ${LIBWEBSOCKETS_LIBRARIES_STATIC})

endif()
## Libwebsockets End
