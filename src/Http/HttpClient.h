﻿/*
 * Copyright (c) 2016-present The ZLMediaKit project authors. All Rights Reserved.
 *
 * This file is part of ZLMediaKit(https://github.com/ZLMediaKit/ZLMediaKit).
 *
 * Use of this source code is governed by MIT-like license that can be found in the
 * LICENSE file in the root of the source tree. All contributing project authors
 * may be found in the AUTHORS file in the root of the source tree.
 */

#ifndef Http_HttpClient_h
#define Http_HttpClient_h

#include <stdio.h>
#include <string.h>
#include <functional>
#include <memory>
#include "Util/util.h"
#include "Util/mini.h"
#include "Network/TcpClient.h"
#include "Common/Parser.h"
#include "HttpRequestSplitter.h"
#include "HttpCookie.h"
#include "HttpChunkedSplitter.h"
#include "Common/strCoding.h"
#include "HttpBody.h"

namespace mediakit {

class HttpArgs : public std::map<std::string, toolkit::variant, StrCaseCompare> {
public:
    std::string make() const {
        std::string ret;
        for (auto &pr : *this) {
            ret.append(pr.first);
            ret.append("=");
            ret.append(strCoding::UrlEncodeComponent(pr.second));
            ret.append("&");
        }
        if (ret.size()) {
            ret.pop_back();
        }
        return ret;
    }
};

class HttpClient : public toolkit::TcpClient, public HttpRequestSplitter {
public:
    using HttpHeader = StrCaseMap;
    using Ptr = std::shared_ptr<HttpClient>;

    /**
     * 发送http[s]请求
     * @param url 请求url
     * Send http[s] request
     * @param url Request url
     
     * [AUTO-TRANSLATED:01b6c9ac]
     */
    virtual void sendRequest(const std::string &url);

    /**
     * 重置对象
     * Reset object
     
     * [AUTO-TRANSLATED:d23b5bbb]
     */
    virtual void clear();

    /**
     * 设置http方法
     * @param method GET/POST等
     * Set http method
     * @param method GET/POST etc.
     
     * [AUTO-TRANSLATED:5199546a]
     */
    void setMethod(std::string method);

    /**
     * 覆盖http头
     * @param header
     * Override http header
     * @param header
     
     * [AUTO-TRANSLATED:ea31a471]
     */
    void setHeader(HttpHeader header);

    HttpClient &addHeader(std::string key, std::string val, bool force = false);

    /**
     * 设置http content
     * @param body http content
     * Set http content
     * @param body http content
     
     * [AUTO-TRANSLATED:9993580c]
     */
    void setBody(std::string body);

    /**
     * 设置http content
     * @param body http content
     * Set http content
     * @param body http content
     
     * [AUTO-TRANSLATED:9993580c]
     */
    void setBody(HttpBody::Ptr body);

    /**
     * 获取回复，在收到完整回复后有效
     * Get response, valid after receiving the complete response
     
     * [AUTO-TRANSLATED:b107995e]
     */
    const Parser &response() const;

    /**
     * 获取回复header声明的body大小
     * Get the body size declared in the response header
     
     * [AUTO-TRANSLATED:65f8e782]
     */
    ssize_t responseBodyTotalSize() const;

    /**
     * 获取已经下载body的大小
     * Get the size of the body that has been downloaded
     
     * [AUTO-TRANSLATED:a3cde7b4]
     */
    size_t responseBodySize() const;

    /**
     * 获取请求url
     * Get the request url
     
     * [AUTO-TRANSLATED:cc7fe537]
     */
    const std::string &getUrl() const;

    /**
     * 判断是否正在等待响应
     * Determine if the response is pending
     
     * [AUTO-TRANSLATED:058719d7]
     */
    bool waitResponse() const;

    /**
     * 判断是否为https
     * Determine if it is https
     
     * [AUTO-TRANSLATED:9b3a0254]
     */
    bool isHttps() const;

    /**
     * 设置从发起连接到接收header完毕的延时，默认10秒
     * 此参数必须大于0
     * Set the delay from initiating the connection to receiving the header, default 10 seconds
     * This parameter must be greater than 0
     
     * [AUTO-TRANSLATED:4cce3e85]
     */
    void setHeaderTimeout(size_t timeout_ms);

    /**
     * 设置接收body数据超时时间, 默认5秒
     * 此参数可以用于处理超大body回复的超时问题
     * 此参数可以等于0
     * Set the timeout for receiving body data, default 5 seconds
     * This parameter can be used to handle timeout issues for large body responses
     * This parameter can be equal to 0
     
     * [AUTO-TRANSLATED:48585852]
     */
    void setBodyTimeout(size_t timeout_ms);

    /**
     * 设置整个链接超时超时时间, 默认0
     * 该值设置不为0后，HeaderTimeout和BodyTimeout无效
     * Set the timeout for the entire link, default 0
     * After this value is set to non-zero, HeaderTimeout and BodyTimeout are invalid
     
     * [AUTO-TRANSLATED:df094868]
     */
    void setCompleteTimeout(size_t timeout_ms);

    /**
     * 设置http代理url
     * Set http proxy url
     
     * [AUTO-TRANSLATED:95df17e7]
     */
    void setProxyUrl(std::string proxy_url);

    /**
     * 当重用连接失败时, 是否允许重新发起请求
     * If the reuse connection fails, whether to allow the request to be resent
     * @param allow true:允许重新发起请求 / true: allow the request to be resent
     * When the reuse connection fails, whether to allow the request to be resent
     * @param allow true: allow the request to be resent
     
     * [AUTO-TRANSLATED:71bd8e67]
     */
    void setAllowResendRequest(bool allow);

protected:
    /**
     * 收到http回复头
     * @param status 状态码，譬如:200 OK
     * @param headers http头
     * Receive http response header
     * @param status Status code, such as: 200 OK
     * @param headers http header
     
     * [AUTO-TRANSLATED:a685f8ef]
     */
    virtual void onResponseHeader(const std::string &status, const HttpHeader &headers) = 0;

    /**
     * 收到http conten数据
     * @param buf 数据指针
     * @param size 数据大小
     * Receive http content data
     * @param buf Data pointer
     * @param size Data size
     
     * [AUTO-TRANSLATED:bee3bf62]
     */
    virtual void onResponseBody(const char *buf, size_t size) = 0;

    /**
     * 接收http回复完毕,
     * Receive http response complete,
     
     * [AUTO-TRANSLATED:b96ed715]
     */
    virtual void onResponseCompleted(const toolkit::SockException &ex) = 0;

    /**
     * 重定向事件
     * @param url 重定向url
     * @param temporary 是否为临时重定向
     * @return 是否继续
     * Redirect event
     * @param url Redirect url
     * @param temporary Whether it is a temporary redirect
     * @return Whether to continue
     * [AUTO-TRANSLATED:b64d5f8b]
     */
    virtual bool onRedirectUrl(const std::string &url, bool temporary) { return true; };

protected:
    //// HttpRequestSplitter override ////
    ssize_t onRecvHeader(const char *data, size_t len) override;
    void onRecvContent(const char *data, size_t len) override;

    //// TcpClient override ////
    void onConnect(const toolkit::SockException &ex) override;
    void onRecv(const toolkit::Buffer::Ptr &pBuf) override;
    void onError(const toolkit::SockException &ex) override;
    void onFlush() override;
    void onManager() override;

    void clearResponse();

    bool checkProxyConnected(const char *data, size_t len);
    bool isUsedProxy() const;
    bool isProxyConnected() const;

private:
    void onResponseCompleted_l(const toolkit::SockException &ex);
    void onConnect_l(const toolkit::SockException &ex);
    void checkCookie(HttpHeader &headers);

private:
    //for http response
    bool _complete = false;
    bool _header_recved = false;
    bool _http_persistent = true;
    bool _allow_resend_request = false;
    size_t _recved_body_size;
    ssize_t _total_body_size;
    Parser _parser;
    std::shared_ptr<HttpChunkedSplitter> _chunked_splitter;

    //for request args
    bool _is_https;
    std::string _url;
    HttpHeader _user_set_header;
    HttpBody::Ptr _body;
    std::string _method;
    std::string _last_host;

    //for this request
    std::string _path;
    HttpHeader _header;

    //for timeout
    size_t _wait_header_ms = 10 * 1000;
    size_t _wait_body_ms = 10 * 1000;
    size_t _wait_complete_ms = 0;
    toolkit::Ticker _wait_header;
    toolkit::Ticker _wait_body;
    toolkit::Ticker _wait_complete;

    bool _used_proxy = false;
    bool _proxy_connected = false;
    uint16_t _proxy_port;
    std::string _proxy_url;
    std::string _proxy_host;
    std::string _proxy_auth;
};

} /* namespace mediakit */

#endif /* Http_HttpClient_h */
