# Porting guide

In order to port the Vertices SDK to other platforms, a few functions need to be redefined.

## HTTP

There are several HTTP libraries that can be used with the Vertices SDK. 
Here is a list of the officially supported stacks, with the current status:

* 🟢 `libcurl`: Unix-based OSes
* 🟢 `esp_http_client` from ESP-IDF
* 🔴 Zephyr native

## Signing

> ⚠️ Not implemented

Signature can be done differently depending on hardware so we are going to add several ways of signing a payload
by overloading a few pre-defined Vertices functions.

