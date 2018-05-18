package com.robotad.waterserver.filter;

import com.google.gson.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.context.annotation.Configuration;

import javax.servlet.*;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.util.Enumeration;

@Configuration
public class ReqResFilter implements Filter {
    private static final Logger log = LoggerFactory.getLogger(ReqResFilter.class);
    private static Gson gson;

    public ReqResFilter() {
        gson = new GsonBuilder().setPrettyPrinting().create();
    }

    @Override
    public void init(FilterConfig filterConfig) throws ServletException {
    }

    @Override
    public void doFilter(ServletRequest request_, ServletResponse response_, FilterChain chain)
            throws ServletException, IOException {
        HttpServletRequest request = (HttpServletRequest) request_;
        HttpServletResponse response = (HttpServletResponse) response_;

        BufferedRequestWrapper b_request = new BufferedRequestWrapper(request);
        BufferedResponseWrapper b_response = new BufferedResponseWrapper(response);

        String request_body = b_request.getRequestBody(1000);

        log.info("Request received: {}", gson.toJson(requestToJSON(request, request_body)));
        chain.doFilter(b_request, b_response);
        log.info("Response sent: {}", gson.toJson(responseToJSON(response, b_response.getContent())));
    }

    @Override
    public void destroy() {
    }

    /**
     * Get a JSON representation of the request
     *
     * @param request
     * @param body    - message-body to include in the log. - If: you do not want to print the message body-pass in another
     *                value
     * @return
     */
    private JsonObject requestToJSON(HttpServletRequest request, String body) {
        JsonObject request_ = new JsonObject();
        request_.addProperty("method", request.getMethod());
        request_.addProperty("request-uri", request.getRequestURI());
        request_.addProperty("request-url", request.getRequestURL().toString());
        request_.addProperty("client-ip", request.getRemoteAddr());


        Gson gson = new Gson();
        JsonElement params_ = gson.toJsonTree(request.getParameterMap());
        request_.add("params", params_);

        Enumeration<String> headers = request.getHeaderNames();

        JsonArray headers_ = new JsonArray();

        while (headers.hasMoreElements()) {
            JsonObject header_ = new JsonObject();
            String headerName = headers.nextElement();
            String headerValue = request.getHeader(headerName);
            header_.addProperty(headerName, headerValue);
            headers_.add(header_);
        }

        if (headers_.size() > 0) {
            request_.add("headers", headers_);
        }

        if (body != null) {
            request_.addProperty("message-body", body);
        } else {
            request_.addProperty("message-body", "null");
        }

        JsonObject request_wrapper_ = new JsonObject();
        request_wrapper_.add("request", request_);
        return request_wrapper_;
    }

    public JsonObject responseToJSON(HttpServletResponse response, String body) {
        JsonObject response_ = new JsonObject();
        response_.addProperty("status-code", response.getStatus());

        JsonArray headers_ = new JsonArray();

        JsonObject contentEncoding = new JsonObject();
        contentEncoding.addProperty("content-encoding", response.getCharacterEncoding());
        headers_.add(contentEncoding);

        JsonObject contentType = new JsonObject();
        contentType.addProperty("content-type", response.getContentType());
        headers_.add(contentType);

        for (String header : response.getHeaderNames()) {
            String value = response.getHeader(header);
            JsonObject header_ = new JsonObject();
            header_.addProperty(header, value);
            headers_.add(header_);
        }

        if (headers_.size() > 0) {
            response_.add("headers", headers_);
        }


        if (body != null) {
            response_.addProperty("message-body", body);
        } else {
            response_.addProperty("message-body", "null");
        }

        JsonObject response_wrapper_ = new JsonObject();
        response_wrapper_.add("response", response_);
        return response_wrapper_;
    }
}
